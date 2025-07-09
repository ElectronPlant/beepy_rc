/**
 * @file      rc.c
 * @brief     Radio Control module - Rx from the radio.
 *
 * @ingroup   Rc
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note Module Prefix: Rc_
 *
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "FreeRTOS.h"
#include "task.h"

#include "target.h"

#include "sbus.h"

/** @addtogroup Rc
 *   @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define RC_TASK_NAME       ("Rc")
#define RC_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define RC_TASK_PRIORITY   (configMAX_PRIORITIES - 2U)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Rc_TaskLoop(void);
static void Rc_TaskMain(PLT_UTILS_UNUSED void *parameters);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
TaskHandle_t         Rc_TaskHandle = NULL;
static SBUS_STATUS_T Rc_SbusState;

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Initialize the Rc module.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1.
 */
bool_t Rc_Init(void) {
    bool_t ok;

    /* Initialize SBUS */
    ok = Sbus_Init();

    /* Start Task */
    if (DEF_TRUE == ok) {
        BaseType_t task_ok = xTaskCreate(
            Rc_TaskMain, RC_TASK_NAME, RC_TASK_STACK_SIZE, NULL, RC_TASK_PRIORITY, &Rc_TaskHandle
        );
        ok = PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(task_ok);
    }

    return ok;
}

/******************************************
 * Task Main
 ******************************************/
static void Rc_TaskLoop(void) {
    printf("--------------\n");
    printf("Start reception\n");
    bool_t ok = Sbus_StartRx();
    PLT_ASSERT(DEF_TRUE == ok);

    vTaskDelay(300); /* delay 300 ticks */

    Sbus_GetFrame(&Rc_SbusState);
    printf("SBUS State: %u\n", Rc_SbusState.State);
    if (SBUS_STATE_OK == Rc_SbusState.State || SBUS_STATE_FRAME_LOST == Rc_SbusState.State
        || SBUS_STATE_FAILSAFE == Rc_SbusState.State) {
        Sbus_DebugFrame(Rc_SbusState.FramePtr);
    } else {
        printf("Frame reception failed\n");
        printf("-----------------\n");
    }
}

static void Rc_TaskMain(PLT_UTILS_UNUSED void *parameters) {
    /* Setup */
    /* --No-op-- */

    /* Loop */
    while (DEF_TRUE) {
        Rc_TaskLoop();
    }
}

/** @} (end addtogroup Rc)   */
