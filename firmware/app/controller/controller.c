/**
 * @file  controller.h
 * @brief Controller - Translates RC channel inputs to actions.
 *
 * @ingroup   Controller
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
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
#include "controller.h"
#include "model.h"
#include "rc_inputs.h"
#include "std_frame.h"
#include "vehicle_int.h"

#include "target.h"

/** @addtogroup Controller
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
/* -- Task -- */
#define CTRLR_TASK_NAME       ("Controller")
#define CTRLR_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define CTRLR_TASK_PRIORITY   (configMAX_PRIORITIES - 3U)

/* -- Timer -- */
#define Ctrlr_TimerHandle_NAME      ("Ctrlr loop")
#define Ctrlr_TimerHandle_PERIOD_MS (250U)
#define Ctrlr_TimerHandle_PERIOD_TICKS \
    ((Ctrlr_TimerHandle_PERIOD_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)
#define CTRLR_NUM_PARALLEL_QUEUE_REQUESTS (5U)
#define CTRLR_TIMEOUT_MS                  (1000u) /* Time between queue updates */
#define CTRLR_TIMEOUT_TICKS ((CTRLR_TIMEOUT_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)

/* -- Task notification -- */
#define CTRLR_BIT_OFFSET_TO_MASK(X) (0x01 << X)

/* -- Failsafe -- */
#define CTRLR_FAILSAFE_CNT_LIMIT (100U) /* Number of RC failsafe frames to trigger a failsafe */

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
    MODEL_RC_SETPOINT_T RcSetPoint; /**< Local snapshot of the latest RC setpoint. */
    uint32_t            PrevNotification;
    uint16_t            FailsafeCnt;
} CTRLR_INFO_T;

typedef enum CTRLR_TASK_NOTICE_OFFSET_E {
    CTRLR_TASK_NOTICE_OFFSET_CONTROL_LOOP = 0,
    CTRLR_TASK_NOTICE_OFFSET_RC,

    CTRLR_TASK_NOTICE_OFFSET_MAX,
} CTRLR_TASK_NOTICE_OFFSET_T;

/* --- FSM --- */
typedef void (*CTRLR_FSM_ACTION_T)(void);
typedef void (*CTRLR_FSM_TRANSITION_T)(CTRLR_STATUS_T);

typedef struct CTRLR_FSM_TABLE_ENTRY_S {
    uint32_t TaskNotificationMask;     /**< Mask for bits to be checked in the task notification */
    CTRLR_FSM_ACTION_T     Action;     /**< Action to process the task notification */
    CTRLR_FSM_TRANSITION_T Transition; /**< Function pointer to handle the state transitions */
} CTRLR_FSM_TABLE_ENTRY_T;


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Ctrlr_TaskMain(PLT_UTILS_UNUSED void* parameters);
static void Ctrlr_LoopTimerCallback(TimerHandle_t xTimer);
static void Ctrlr_HandleModelUpdate(MODEL_NOTIFY_SOURCE_T source);

static void Ctrlr_UpdateState(CTRLR_STATUS_T status);
static void Ctrlr_ActionAssert(void);
static void Ctrlr_ActionNone(void);
static void Ctrlr_TransitionApply(CTRLR_STATUS_T status);
static void Ctrlr_WaitForDisarmAction(void);
static void Ctrlr_DisarmedAction(void);
static void Ctrlr_DisarmedTransition(CTRLR_STATUS_T status);
static void Ctrlr_RunningAction(void);
static void Ctrlr_RunningTransition(CTRLR_STATUS_T status);

/********************************************************************************
 * Local Vars
 ********************************************************************************/
static TaskHandle_t  Ctrlr_TaskHandle = NULL;
static TimerHandle_t Ctrlr_TimerHandle = NULL;

static CTRLR_INFO_T Ctrlr_Info = {
    .Status = CTRLR_STATUS_UNINITIALIZED,
    .RcSetPoint.State = MODEL_RC_SETPOINT_STATE_PENDING,
    .PrevNotification = 0x00000000,
    .FailsafeCnt = 0U,
};

static CTRLR_FSM_TABLE_ENTRY_T Ctrlr_FsmTable[CTRLR_STATUS_MAX] = {
    /* UNINITIALIZED */
    {.TaskNotificationMask = UINT32_MAX,
     .Action = Ctrlr_ActionAssert,
     .Transition = Ctrlr_TransitionApply},

    /* STOPPED       */
    {.TaskNotificationMask = UINT32_MAX,
     .Action = Ctrlr_ActionAssert,
     .Transition = Ctrlr_TransitionApply},

    /* PREARMED      */
    {.TaskNotificationMask = CTRLR_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC),
     .Action = Ctrlr_WaitForDisarmAction,
     .Transition = Ctrlr_TransitionApply},

    /* DISARMED      */
    {.TaskNotificationMask = CTRLR_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC),
     .Action = Ctrlr_DisarmedAction,
     .Transition = Ctrlr_DisarmedTransition},

    /* RUNNING      */
    {.TaskNotificationMask = CTRLR_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_CONTROL_LOOP),
     .Action = Ctrlr_RunningAction,
     .Transition = Ctrlr_RunningTransition},

    /* FAILSAFE      */
    {.TaskNotificationMask = CTRLR_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC),
     .Action = Ctrlr_WaitForDisarmAction,
     .Transition = Ctrlr_TransitionApply},
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
        Ctrlr_TimerHandle_NAME,
        Ctrlr_TimerHandle_PERIOD_TICKS,
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
        CTRLR_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_CONTROL_LOOP),
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
            xTaskNotify(
                Ctrlr_TaskHandle,
                CTRLR_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC),
                eSetBits
            );
            break;
        case MODEL_NOTIFY_SOURCE_POS:
        case MODEL_NOTIFY_SOURCE_ATTITUDE:
            // TODO not implemented.
            break;
        default:
            PLT_UNREACHABLE;
    }
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
    CTRLR_FSM_TRANSITION_T p_funct = Ctrlr_FsmTable[Ctrlr_Info.Status].Transition;
    PLT_ASSERT(NULL != p_funct);
    p_funct(status);
}

/**
 * @brief  Action to assert. Reserved for when a state must not execute an action.
 */
static void Ctrlr_ActionAssert(void) {
    PLT_UNREACHABLE;
}

/**
 * @brief  Ignores the action.
 */
static void Ctrlr_ActionNone(void) {
    /* - No-op - */
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
    BaseType_t ok = xTimerStart(Ctrlr_TimerHandle, CTRLR_TIMEOUT_TICKS);
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
        && DEF_FALSE == Ctrlr_Info.RcSetPoint.ArmSwitch) {
        Ctrlr_UpdateState(CTRLR_STATUS_DISARMED);
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
        && DEF_TRUE == Ctrlr_Info.RcSetPoint.ArmSwitch) {
        Ctrlr_UpdateState(CTRLR_STATUS_RUNNING);
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
 * @brief  Checks the validity of the RC setpoint and handles any errors.
 *
 * @return DEF_TRUE if the RC frame is valid, DEF_FALSE otherwise.
 */
static bool_t Ctrlr_HandleRcErrors(void) {
    bool_t   rc_error = DEF_FALSE;
    uint32_t mask = CTRLR_BIT_OFFSET_TO_MASK(CTRLR_TASK_NOTICE_OFFSET_RC);
    if (0U != (ulTaskNotifyValueClear(Ctrlr_TaskHandle, mask) & mask)) {
        Model_GetRcSetpoint(&Ctrlr_Info.RcSetPoint);
        switch (Ctrlr_Info.RcSetPoint.State) {
            case MODEL_RC_SETPOINT_STATE_VALID:
                Ctrlr_Info.FailsafeCnt = 0;
                break;
            case MODEL_RC_SETPOINT_STATE_FAILSAFE:
                Ctrlr_Info.FailsafeCnt++;
                if (CTRLR_FAILSAFE_CNT_LIMIT >= Ctrlr_Info.FailsafeCnt) {
                    rc_error = DEF_TRUE;
                }
            default:
                rc_error = DEF_TRUE;
        }
    }
    return rc_error;
}

/**
 * @brief  Runs the control loop.
 */
static void Ctrlr_RunControlLoop(void) {
    CVInt_Interface.VInt_RunControlLoop(&Ctrlr_Info.RcSetPoint);
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
    bool_t rc_error = Ctrlr_HandleRcErrors();
    if (DEF_TRUE == rc_error) {
        Ctrlr_UpdateState(CTRLR_STATUS_FAILSAFE);
        Ctrlr_Info.RcSetPoint.Throttle = 0.0f;
        Ctrlr_Info.RcSetPoint.Yaw = 0.0f;
        Ctrlr_StopMotors();
    } else if (DEF_FALSE == Ctrlr_Info.RcSetPoint.ArmSwitch) {
        Ctrlr_UpdateState(CTRLR_STATUS_DISARMED);
        Ctrlr_StopMotors();
    } else {
        /* --- Control loop --- */
        Ctrlr_RunControlLoop();
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
            Ctrlr_StopMotors();
            break;
        case CTRLR_STATUS_DISARMED:
            Ctrlr_StopMotors();
            break;
        default:
            PLT_UNREACHABLE;
    }
    Ctrlr_TransitionApply(status);
    Ctrlr_DisableControlLoopTimer();
}


/******************************************
 * Task Main
 ******************************************/
/**
 * @brief  Starts the controller task.
 */
static void Ctrlr_TaskStart(void) {
    CVInt_Interface.VInt_Start();
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
            &Ctrlr_Info.PrevNotification,
            CTRLR_TIMEOUT_TICKS
        );
        received = PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(received_int);
    } else {
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

    /* Loop */
    while (DEF_TRUE) {
        Ctrlr_TaskLoop();
    }
}

/** @} (end addtogroup Controller)  */
