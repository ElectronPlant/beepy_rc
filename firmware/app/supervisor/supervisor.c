/**
 * @file  supervisor.c
 * @brief Supervisor task to control User Interface (UI) and internal state.
 *
 * @ingroup   Supervisor
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Super_
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "sound.h"
#include "supervisor.h"
#include "supervisor_defines.h"
#include "ui.h"

#include "controller.h"


/** @addtogroup App
 *    @{
 */

/** @addtogroup Supervisor
 *    @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
/* -- Task -- */
#define SUPER_TASK_NAME       ("Super")
#define SUPER_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define SUPER_TASK_PRIORITY   (configMAX_PRIORITIES - 1U)

/* -- Timer -- */
#define SUPER_TIMER_HANDLE_NAME ("Super")
#define SUPER_TIMER_PERIOD_MS   (250U)
#define SUPER_TIMER_PERIOD_TICKS \
    ((SUPER_TIMER_PERIOD_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)
#define SUPER_TIMEOUT_MS    (1000u) /* Time between queue updates */
#define SUPER_TIMEOUT_TICKS ((SUPER_TIMEOUT_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)


/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum SUPER_STATUS_E {
    SUPER_STATUS_UNINITIALIZED = 0,
    SUPER_STATUS_INITIALIZED,
    SUPER_STATUS_RUNNING,
} SUPER_STATUS_T;

typedef struct SUPER_INFO_S {
    SUPER_STATUS_T Status;
    uint32_t       ExternalContextMap;
} SUPER_INFO_T;


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Super_TimerCallback(PLT_UTILS_UNUSED TimerHandle_t xTimer);

static void Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_T notice);

static void Super_TaskStart(void);
static void Super_TaskLoop(void);
static void Super_TaskMain(PLT_UTILS_UNUSED void* parameters);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
static TaskHandle_t  Super_TaskHandle = NULL;
static TimerHandle_t Super_TimerHandle = NULL;

static SUPER_INFO_T Super_Info = {.Status = SUPER_STATUS_UNINITIALIZED, .ExternalContextMap = 0};


/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
  * @brief  Initialization function for the supervisor task.
  *
  * @return DEF_TRUE if successful, DEF_FALSE otherwise.
  */
bool_t Super_Init(void) {
    bool_t ok;

    PLT_BUILD_ASSERT(32U >= SUPER_SOURCE_MAX);
    PLT_ASSERT(SUPER_STATUS_UNINITIALIZED == Super_Info.Status); /* Force singleton */

    /* Init Task */
    BaseType_t task_ok = xTaskCreate(
        Super_TaskMain,
        SUPER_TASK_NAME,
        SUPER_TASK_STACK_SIZE,
        NULL,
        SUPER_TASK_PRIORITY,
        &Super_TaskHandle
    );
    ok = PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(task_ok);

    /* Init Timer */
    if (DEF_TRUE == ok) {
        Super_TimerHandle = xTimerCreate(
            SUPER_TIMER_HANDLE_NAME,
            SUPER_TIMER_PERIOD_TICKS,
            pdTRUE,
            NULL,
            Super_TimerCallback
        );
        ok = NULL != Super_TimerHandle ? DEF_TRUE : DEF_FALSE;
    }

    if (DEF_TRUE == ok) {
        ok = Ui_Init();
    }

    if (DEF_TRUE == ok) {
        ok = Sound_Init();
    }

    if (DEF_TRUE == ok) {
        Super_Info.Status = SUPER_STATUS_INITIALIZED;
    }

    return ok;
}


/******************************************
 * Context Gathering
 ******************************************/

/**
 * @brief  Implementation of a generic ISR callback to send task notifications.
 *
 * @param  offset
 */
static void Super_GenericIsrCallback(SUPERDEF_SOURCE_OFFSET_T offset) {
    BaseType_t higher_priority_task_awaken = pdFALSE;

    (void)xTaskNotifyFromISR(
        Super_TaskHandle,
        PLT_UTILS_BIT_OFFSET_TO_MASK(offset),
        eSetBits,
        &higher_priority_task_awaken
    );
    portYIELD_FROM_ISR(higher_priority_task_awaken);
}

/**
 * @brief  Callback function for the Controller timer.
 *         Sends the control loop timer task notification.
 *
 * @param  xTimer Pointer to the timer that caused the callback.
 */
static void Super_TimerCallback(PLT_UTILS_UNUSED TimerHandle_t xTimer) {
    Super_GenericIsrCallback(SUPERDEF_SOURCE_OFFSET_TIMER);
}

/**
  * @brief  Notifies that button 1 has been pressed.
  */
void Super_NotifyButton1(void) {
    Super_GenericIsrCallback(SUPERDEF_SOURCE_OFFSET_BUTTON_1);
}

/**
  * @brief  Notifies that button 2 has been pressed..
  */
void Super_NotifyButton2(void) {
    Super_GenericIsrCallback(SUPERDEF_SOURCE_OFFSET_BUTTON_2);
}

/**
  * @brief  Notifies that the RC task is running.
  */
void Super_NotifyRcRunning(void) {
    Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_RC_ALIGNED);
}

/**
  * @brief  Notifies that the RC task is running.
  */
void Super_NotifyRcDisconnected(void) {
    Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_RC_DISCONNECTED);
}

/**
  * @brief  Notifies that the RC task is running.
  */
void Super_NotifyRcReconnected(void) {
    Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_RC_RECONNECTED);
}

/**
  * @brief  Notifies that the RC task has had an error.
  */
void Super_NotifyRcError(void) {
    Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_RC_ERROR);
}

/**
  * @brief  Notifies that the vehicle is armed.
  */
void Super_NotifyArmed(void) {
    Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_ARMED);
}

/**
  * @brief  Notifies that the vehicle is disarmed.
  */
void Super_NotifyDisarmed(void) {
    Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_DISARMED);
}

/**
  * @brief  Notifies that the controller is running.
  */
void Super_NotifyControllerOk(void) {
    Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_CONTROLLER_OK);
}

/**
  * @brief  Notifies that the controller has had an error.
  */
void Super_NotifyControllerError(void) {
    Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_CONTROLLER_ERROR);
}

/**
 * @brief  Notifies a new context event.
 *
 * @param  Context event to notify.
 */
static void Super_ContextNotice(SUPERDEF_SOURCE_OFFSET_T notice) {
    PLT_ASSERT(SUPER_SOURCE_MAX > notice);

    BaseType_t ok = xTaskNotify(Super_TaskHandle, PLT_UTILS_BIT_OFFSET_TO_MASK(notice), eSetBits);
    PLT_ASSERT(DEF_TRUE == PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(ok));
}

/******************************************
 * Task Main
 ******************************************/
/**
 * @brief  Update the status.
 */
static void Super_UpdateState(SUPER_STATUS_T status) {
    Super_Info.Status = status;
}

/**
 * @brief  Starts the controller task.
 */
static void Super_TaskStart(void) {
    BaseType_t tim_ok = xTimerStart(Super_TimerHandle, SUPER_TIMEOUT_TICKS);
    PLT_ASSERT(DEF_TRUE == PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(tim_ok));

    Ui_Start();
    Sound_StartMelody(SOUND_MELODIES_INIT);
    Super_UpdateState(SUPER_STATUS_RUNNING);
}

/**
 * @brief  Processes the notified context update.
 *         It uses this information to update the UI.
 *
 * @param  notifications Notification status.
 */
static void Super_ProcessContextUpdate(uint32_t notifications) {
    bool_t timer_update = PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_TIMER);

    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_RC_ALIGNED)) {
        Super_Info.ExternalContextMap &=
            ~PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_ERROR);
        Super_Info.ExternalContextMap |=
            PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_ALIGNED);
        Super_Info.ExternalContextMap |=
            PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_CONNECTED);
    }
    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_RC_ERROR)) {
        Super_Info.ExternalContextMap |=
            PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_ERROR);
        Super_Info.ExternalContextMap &=
            ~PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_ALIGNED);
        Super_Info.ExternalContextMap &=
            ~PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_CONNECTED);
    }
    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_RC_DISCONNECTED)) {
        Super_Info.ExternalContextMap &=
            ~PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_CONNECTED);
    }
    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_RC_RECONNECTED)) {
        Super_Info.ExternalContextMap |=
            PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_CONNECTED);
        Super_Info.ExternalContextMap &=
            ~PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_RC_ERROR);
    }
    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_ARMED)) {
        Super_Info.ExternalContextMap |=
            PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_ARMED);
        Sound_StartMelody(SOUND_MELODIES_ARM);
    }
    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_DISARMED)) {
        Super_Info.ExternalContextMap &=
            ~PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_ARMED);
        Sound_StartMelody(SOUND_MELODIES_DISARM);
    }
    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_CONTROLLER_OK)) {
        Super_Info.ExternalContextMap &=
            ~PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_CONTROLLER_ERROR);
    }
    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_CONTROLLER_ERROR)) {
        Super_Info.ExternalContextMap |=
            PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_CONTEXT_OFFSET_CONTROLLER_ERROR);
    }

    Ui_SetStatusLeds(Super_Info.ExternalContextMap, timer_update);
}

/**
 * @brief  Handle the button actions.
 *
 * @param  notifications Task notifications.
 */
void Super_ButtonActions(uint32_t notifications) {
    Ui_RunButtonActions(notifications);
}

/**
 * @brief  Loop for the controller task.
 *         Waits for the task notifications, performs the actions, and handles the FSM.
 */
static void Super_TaskLoop(void) {
    uint32_t mask = UINT32_MAX;
    uint32_t notifications;

    BaseType_t received_int = xTaskNotifyWait(0, mask, &notifications, SUPER_TIMEOUT_TICKS);
    if (DEF_TRUE == PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(received_int)) {
        printf("Supervisor::Task running, %lu\n", notifications);
        if (0 != (notifications & ~SUPERDEF_SOURCE_BUTTONS_MASK)) {
            Super_ProcessContextUpdate(notifications);
        }
        if (0 != (notifications & SUPERDEF_SOURCE_BUTTONS_MASK)) {
            Super_ButtonActions(notifications);
        }
    }
}

/**
 * @brief  Controller task function.
 *
 * @param  task parameters, it won't be used.
 */
static void Super_TaskMain(PLT_UTILS_UNUSED void* parameters) {
    /* Setup */
    Super_TaskStart();

    /* Loop */
    while (DEF_TRUE) {
        Super_TaskLoop();
    }
}


/** @} (end addtogroup Supervisor)  */
/** @} (end addtogroup App)         */
