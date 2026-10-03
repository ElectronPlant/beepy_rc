/**
 * @file  controller.h
 * @brief Controller - Translates RC channel inputs to actions.
 *
 * @ingroup   Controller
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Ctrlr_
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include "common_vehicle_int.h"
#include "control_inputs.h"
#include "controller.h"
#include "curves.h"
#include "model.h"
#include "rc_subs.h"
#include "std_frame.h"
#include "vehicle_int.h"

#include "supervisor.h"

#include "target.h"

/** @addtogroup Controller
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
/* -- Task -- */
#define CTRLR_TASK_NAME       ("Ctrlr")
#define CTRLR_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define CTRLR_TASK_PRIORITY   (configMAX_PRIORITIES - 3U)

/* -- Timer -- */
#define CTRLR_TIMER_HANDLE_NAME ("Ctrlr")
#define CTRLR_TIMER_PERIOD_MS   (250U)
#define CTRLR_TIMER_PERIOD_TICKS \
    ((CTRLR_TIMER_PERIOD_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)
#define CTRLR_TIMEOUT_MS    (1000u) /* Time between queue updates */
#define CTRLR_TIMEOUT_TICKS ((CTRLR_TIMEOUT_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)

/* --- Disarm --- */
#define CTRLR_DISARM_FRAMES (100U)

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum CTRLR_STATUS_E {
    CTRLR_STATUS_UNINITIALIZED = 0,
    CTRLR_STATUS_STOPPED,
    CTRLR_STATUS_PREARMED,
    CTRLR_STATUS_DISARMED,
    CTRLR_STATUS_RUNNING,
    CTRLR_STATUS_FAILSAFE,

    CTRLR_STATUS_MAX,
} CTRLR_STATUS_T;

typedef struct CTRLR_INFO_S {
    CTRLR_STATUS_T      Status;
    MODEL_RC_SETPOINT_T RcSetPoint;       /**< Local snapshot of the latest RC setpoint. */
    uint32_t            NotVal;           /**< Latest Task notification value */
    uint16_t            DisarmedFrameCnt; /**< Number of disarmed frames */
} CTRLR_INFO_T;

typedef enum CTRLR_TASK_NOTICE_OFFSET_E {
    CTRLR_TASK_NOTICE_OFFSET_CONTROL_LOOP = 0,
    CTRLR_TASK_NOTICE_OFFSET_RC,
    CTRLR_TASK_NOTICE_OFFSET_BUTTON_DISARM,

    CTRLR_TASK_NOTICE_OFFSET_MAX,
} CTRLR_TASK_NOTICE_OFFSET_T;

/* --- FSM --- */
typedef void (*CTRLR_FSM_ACTION_T)(void);
typedef void (*CTRLR_FSM_TRANSITION_T)(CTRLR_STATUS_T);
typedef void (*CTRLR_FSM_INIT_STATE_T)(void);

typedef struct CTRLR_FSM_TABLE_ENTRY_S {
    uint32_t TaskNotificationMask;     /**< Mask for bits to be checked in the task notification */
    CTRLR_FSM_ACTION_T     Action;     /**< Action to process the task notification */
    CTRLR_FSM_TRANSITION_T Transition; /**< Function pointer to handle the state transitions */
    CTRLR_FSM_INIT_STATE_T Init;       /** Initialize state. */

} CTRLR_FSM_TABLE_ENTRY_T;


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Ctrlr_TaskMain(PLT_UTILS_UNUSED void* parameters);
static void Ctrlr_LoopTimerCallback(TimerHandle_t xTimer);
static void Ctrlr_HandleModelUpdate(MODEL_NOTIFY_SOURCE_T source);

static void Ctrlr_UpdateState(CTRLR_STATUS_T status);
static void Ctrlr_ActionAssert(void);
static void Ctrlr_TransitionApply(CTRLR_STATUS_T status);
static void Ctrlr_WaitForDisarmAction(void);
static void Ctrlr_DisarmedAction(void);
static void Ctrlr_DisarmedTransition(CTRLR_STATUS_T status);
static void Ctrlr_RunningAction(void);
static void Ctrlr_RunningTransition(CTRLR_STATUS_T status);

static void Ctrlr_InitStateNoop(void);
static void Ctrlr_InitStateResetDisarmCnt(void);

/********************************************************************************
 * Local Vars
 ********************************************************************************/
static TaskHandle_t  Ctrlr_TaskHandle = NULL;
static TimerHandle_t Ctrlr_TimerHandle = NULL;

static CTRLR_INFO_T Ctrlr_Info = {
    .Status = CTRLR_STATUS_UNINITIALIZED,
    .RcSetPoint.State = MODEL_RC_SETPOINT_STATE_PENDING,
    .NotVal = 0x00000000,
    .DisarmedFrameCnt = 0,
};

static CTRLR_FSM_TABLE_ENTRY_T Ctrlr_FsmTable[CTRLR_STATUS_MAX] = {
    /* UNINITIALIZED */
    {.TaskNotificationMask = UINT32_MAX,
     .Action = Ctrlr_ActionAssert,
     .Transition = Ctrlr_TransitionApply,
     .Init = Ctrlr_InitStateNoop},

    /* STOPPED       */
    {.TaskNotificationMask = UINT32_MAX,
     .Action = Ctrlr_ActionAssert,
     .Transition = Ctrlr_TransitionApply,
     .Init = Ctrlr_InitStateNoop},

    /* PREARMED      */
    {.TaskNotificationMask = PLT_UTILS_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC),
     .Action = Ctrlr_WaitForDisarmAction,
     .Transition = Ctrlr_TransitionApply,
     .Init = Ctrlr_InitStateResetDisarmCnt},

    /* DISARMED      */
    {.TaskNotificationMask = PLT_UTILS_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC),
     .Action = Ctrlr_DisarmedAction,
     .Transition = Ctrlr_DisarmedTransition,
     .Init = Ctrlr_InitStateNoop},

    /* RUNNING      */
    {.TaskNotificationMask = PLT_UTILS_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_CONTROL_LOOP),
     .Action = Ctrlr_RunningAction,
     .Transition = Ctrlr_RunningTransition,
     .Init = Ctrlr_InitStateNoop},

    /* FAILSAFE      */
    {.TaskNotificationMask = PLT_UTILS_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC),
     .Action = Ctrlr_WaitForDisarmAction,
     .Transition = Ctrlr_TransitionApply,
     .Init = Ctrlr_InitStateResetDisarmCnt},
};


/********************************************************************************
 * Function Implementations
 ********************************************************************************/
/**
 * @brief  Initialize the controller module.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Ctrlr_Init(void) {
    bool_t ok;

    PLT_ASSERT(CTRLR_STATUS_UNINITIALIZED == Ctrlr_Info.Status); /* Force singleton */

    /* Init Timer */
    Ctrlr_TimerHandle = xTimerCreate(
        CTRLR_TIMER_HANDLE_NAME,
        CTRLR_TIMER_PERIOD_TICKS,
        pdTRUE,
        NULL,
        Ctrlr_LoopTimerCallback
    );
    ok = NULL != Ctrlr_TimerHandle ? DEF_TRUE : DEF_FALSE;

    /* Init Task */
    if (DEF_TRUE == ok) {
        BaseType_t task_ok = xTaskCreate(
            Ctrlr_TaskMain,
            CTRLR_TASK_NAME,
            CTRLR_TASK_STACK_SIZE,
            NULL,
            CTRLR_TASK_PRIORITY,
            &Ctrlr_TaskHandle
        );
        ok = PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(task_ok);
    }

    /* Init vehicle */
    if (DEF_TRUE == ok) {
        ComVInt_CheckInterface();
        ok = CVInt_Interface.VInt_Init();
    }

    /* Init model */
    if (DEF_TRUE == ok) {
        ok = Model_Init(Ctrlr_HandleModelUpdate);
    }

    /* Init Aux peripherals */
    if (DEF_TRUE == ok) {
        ok = CIn_Init();
    }

    /* State */
    if (DEF_TRUE == ok) {
        Ctrlr_UpdateState(CTRLR_STATUS_STOPPED);
    }

    return ok;
}

/**
 * @brief  Callback function for the loop timer.
 *         Sends the control loop timer task notification.
 *
 * @param  xTimer Pointer to the timer that caused the callback.
 */
static void Ctrlr_LoopTimerCallback(PLT_UTILS_UNUSED TimerHandle_t xTimer) {
    BaseType_t higher_priority_task_awaken = pdFALSE;

    (void)xTaskNotifyFromISR(
        Ctrlr_TaskHandle,
        PLT_UTILS_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_CONTROL_LOOP),
        eSetBits,
        &higher_priority_task_awaken
    );
    portYIELD_FROM_ISR(higher_priority_task_awaken);
}

/**
 * @brief  Callback function to update the model data.
 *
 * @param  source Source that is updating the model.
 */
static void Ctrlr_HandleModelUpdate(MODEL_NOTIFY_SOURCE_T source) {
    switch (source) {
        case MODEL_NOTIFY_SOURCE_RC:
            BaseType_t ok = xTaskNotify(
                Ctrlr_TaskHandle,
                PLT_UTILS_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC),
                eSetBits
            );
            PLT_ASSERT(DEF_TRUE == PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(ok));
            break;
        case MODEL_NOTIFY_SOURCE_POS:
        case MODEL_NOTIFY_SOURCE_ATTITUDE:
            // TODO not implemented.
            break;
        default:
            PLT_UNREACHABLE;
    }
}

/**
 * @brief  Callback function to update the model data.
 *
 * @param  source Source that is updating the model.
 */
void Ctrlr_HandleButtonDisarm(void) {
    BaseType_t higher_priority_task_awaken = pdFALSE;

    (void)xTaskNotifyFromISR(
        Ctrlr_TaskHandle,
        PLT_UTILS_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_BUTTON_DISARM),
        eSetBits,
        &higher_priority_task_awaken
    );
    portYIELD_FROM_ISR(higher_priority_task_awaken);
}


static bool_t Ctrlr_GetArmSwitch(MODEL_RC_SETPOINT_T* p_setpoint) {
    return Curves_Analog2Dig(p_setpoint->DriveInputs[RCSUBS_DRIVE_SETPOINT_ARM_SWITCH]);
}

/******************************************
 * Actions
 ******************************************/
/**
 * @brief  Updates the state and handles the transition actions.
 *
 * @param  status New status to be set.
 */
static void Ctrlr_UpdateState(CTRLR_STATUS_T status) {
    /* Transition from current state */
    CTRLR_FSM_TRANSITION_T p_transition_funct = Ctrlr_FsmTable[Ctrlr_Info.Status].Transition;
    PLT_ASSERT(NULL != p_transition_funct);
    p_transition_funct(status);

    /* Initialize current state */
    CTRLR_FSM_INIT_STATE_T p_init_funct = Ctrlr_FsmTable[status].Init;
    PLT_ASSERT(NULL != p_init_funct);
    p_init_funct();
}

/**
 * @brief  Action to assert. Reserved for when a state must not execute an action.
 */
static void Ctrlr_ActionAssert(void) {
    PLT_UNREACHABLE;
}

/**
 * @brief Transition action to just apply the new state.
 */
static void Ctrlr_TransitionApply(CTRLR_STATUS_T status) {
    Ctrlr_Info.Status = status;
}

/**
 * @brief  Starts the Control loop timer.
 */
static void Ctrlr_EnableControlLoopTimer(void) {
    BaseType_t ok = xTimerStart(Ctrlr_TimerHandle, CTRLR_TIMEOUT_TICKS);
    PLT_ASSERT(pdPASS == ok);
}

/**
 * @brief  Stops the Control loop timer.
 */
static void Ctrlr_DisableControlLoopTimer(void) {
    BaseType_t ok = xTimerStop(Ctrlr_TimerHandle, CTRLR_TIMEOUT_TICKS);
    PLT_ASSERT(pdPASS == ok);
}

/**
 * @brief  Waits for a valid RC frame with the arm switch disabled, to transition to the disabled
 *         state.
 */
static void Ctrlr_WaitForDisarmAction(void) {
    /* Update RC model */
    Model_GetRcSetpoint(&Ctrlr_Info.RcSetPoint);
    if (Ctrlr_Info.RcSetPoint.State == MODEL_RC_SETPOINT_STATE_VALID
        && DEF_FALSE == Ctrlr_GetArmSwitch(&Ctrlr_Info.RcSetPoint)) {
        if (CTRLR_DISARM_FRAMES <= Ctrlr_Info.DisarmedFrameCnt++) {
            Ctrlr_UpdateState(CTRLR_STATUS_DISARMED);
        }
    }
}

/**
 * @brief  Notification action for the disarmed state.
 *         Waits for a valid RC frame with the arm switch enabled.
 */
static void Ctrlr_DisarmedAction(void) {
    /* Update RC model */
    Model_GetRcSetpoint(&Ctrlr_Info.RcSetPoint);
    if (Ctrlr_Info.RcSetPoint.State == MODEL_RC_SETPOINT_STATE_VALID
        && DEF_TRUE == Ctrlr_GetArmSwitch(&Ctrlr_Info.RcSetPoint)) {
        Ctrlr_UpdateState(CTRLR_STATUS_RUNNING);
        Super_NotifyArmed();
        printf("Controller :: Armed!!\n");
    }
}

/**
 * @brief  Transition from the disarmed state to the running state.
 *         Also enables the control loop timer.
 */
static void Ctrlr_DisarmedTransition(CTRLR_STATUS_T status) {
    PLT_ASSERT(CTRLR_STATUS_RUNNING == status);
    Ctrlr_TransitionApply(CTRLR_STATUS_RUNNING);
    Ctrlr_EnableControlLoopTimer();
}

/**
 * @brief  Updates the Rc Setpoint if applicable.
 */
static void Ctrlr_HandleRcSetpoint(void) {
    uint32_t mask = PLT_UTILS_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC);
    if (0U != (ulTaskNotifyValueClear(Ctrlr_TaskHandle, mask) & mask)) {
        Model_GetRcSetpoint(&Ctrlr_Info.RcSetPoint);
    }
}

/**
 * @brief  Runs the control loop.
 */
static void Ctrlr_RunControlLoop(void) {
    CVInt_Interface.VInt_RunControlLoop(&Ctrlr_Info.RcSetPoint);
    CIn_RunAux(&Ctrlr_Info.RcSetPoint);
}

/**
 * @brief  Disarm motors.
 */
static void Ctrlr_StopMotors(void) {
    CVInt_Interface.VInt_Disarm();
}

/**
 * @brief  Notification action for the running state.
 *         It additionally manages RC errors and disarming.
 */
static void Ctrlr_RunningAction(void) {
    /* Update RC model */
    Ctrlr_HandleRcSetpoint();
    bool_t disarmed = (DEF_FALSE == Ctrlr_GetArmSwitch(&Ctrlr_Info.RcSetPoint))
        || (PLT_UTILS_IS_BIT_OFFSET_SET(Ctrlr_Info.NotVal, CTRLR_TASK_NOTICE_OFFSET_BUTTON_DISARM));

    switch (Ctrlr_Info.RcSetPoint.State) {
        case MODEL_RC_SETPOINT_STATE_VALID:
            if (disarmed) {
                CTRLR_STATUS_T new_status = DEF_FALSE == Ctrlr_GetArmSwitch(&Ctrlr_Info.RcSetPoint)
                    ? CTRLR_STATUS_DISARMED
                    : CTRLR_STATUS_PREARMED;
                Ctrlr_UpdateState(new_status);
            } else {
                /* --- Control loop --- */
                Ctrlr_RunControlLoop();
            }
            break;
        case MODEL_RC_SETPOINT_STATE_DISCONNECTED:
            Ctrlr_UpdateState(CTRLR_STATUS_FAILSAFE);
            break;
        default:
            PLT_UNREACHABLE;
    }
}

/**
 * @brief  Transition action from the running state.
 *
 * @param  status New state to transition to.
 */
static void Ctrlr_RunningTransition(CTRLR_STATUS_T status) {
    switch (status) {
        case CTRLR_STATUS_FAILSAFE:
        case CTRLR_STATUS_DISARMED:
        case CTRLR_STATUS_PREARMED:
            Ctrlr_StopMotors();
            Super_NotifyDisarmed();
            Ctrlr_Info.DisarmedFrameCnt = 0;
            break;
        default:
            PLT_UNREACHABLE;
    }
    Ctrlr_TransitionApply(status);
    Ctrlr_DisableControlLoopTimer();
}

/**
 * @brief  Transition Init, do nothing. This is the default action.
 */
static void Ctrlr_InitStateNoop(void) {
    /* - No-op - */
}

/**
 * @brief  Transition Init, reset disarm frame cnt.
 */
static void Ctrlr_InitStateResetDisarmCnt(void) {
    Ctrlr_Info.DisarmedFrameCnt = 0;
}


/******************************************
 * Task Main
 ******************************************/
/**
 * @brief  Starts the controller task.
 */
static void Ctrlr_TaskStart(void) {
    CVInt_Interface.VInt_Start();
    CIn_Start();
    Ctrlr_UpdateState(CTRLR_STATUS_PREARMED);
}

/**
 * @brief  Loop for the controller task.
 *         Waits for the task notifications, performs the actions, and handles the FSM.
 */
static void Ctrlr_TaskLoop(void) {
    bool_t received = DEF_FALSE;

    uint32_t mask = Ctrlr_FsmTable[Ctrlr_Info.Status].TaskNotificationMask;
    uint32_t current = ulTaskNotifyValueClear(Ctrlr_TaskHandle, mask);
    if (0 == (current & mask)) { /* Check if it was already received */
        BaseType_t received_int = xTaskNotifyWait(
            0x00, /* Don't clear any bits on entry. */
            mask,
            &Ctrlr_Info.NotVal,
            CTRLR_TIMEOUT_TICKS
        );
        received = PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(received_int);
    } else {
        Ctrlr_Info.NotVal = current;
        received = DEF_TRUE;
    }

    if (DEF_TRUE == received) {
        CTRLR_FSM_ACTION_T p_action = Ctrlr_FsmTable[Ctrlr_Info.Status].Action;
        PLT_ASSERT(NULL != p_action);
        p_action();
    }
}

/**
 * @brief  Controller task function.
 *
 * @param  task parameters, it won't be used.
 */
static void Ctrlr_TaskMain(PLT_UTILS_UNUSED void* parameters) {
    /* Setup */
    Ctrlr_TaskStart();
    printf("Controller started\n");

    /* Loop */
    while (DEF_TRUE) {
        Ctrlr_TaskLoop();
    }
}

/** @} (end addtogroup Controller)  */
