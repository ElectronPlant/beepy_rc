/**
 * @file      main.c
 * @brief     Main.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note Module Prefix: Main_
 *
 */

#include "FreeRTOS.h"
#include "string.h"
#include "task.h"

#include "plt_assert.h"

#include "bsp.h"
#include "controller.h"
#include "rc.h"
#include "supervisor.h"


/** @addtogroup App
 *   @{
 */

/** @addtogroup Main
 *   @{
 */

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief Initializes the hardware components.
 */
static void Main_InitBoard(void) {
    bool_t ok;

    ok = BSP_Init();
    PLT_ASSERT(DEF_TRUE == ok);
}

/**
 * @brief  Initializes the application modules.
 */
static void Main_InitModules(void) {
    bool_t ok;

    /* Rx module */
    ok = Rc_Init();
    PLT_ASSERT(DEF_TRUE == ok);

    /* Motion module */
    ok = Ctrlr_Init();
    PLT_ASSERT(DEF_TRUE == ok);

    /* Supervisor module */
    ok = Super_Init();
    PLT_ASSERT(DEF_TRUE == ok);
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* Start up */
    Main_InitBoard();
    Main_InitModules();

    /* Run scheduler */
    vTaskStartScheduler();
    PLT_UNREACHABLE;
}


/******************************************
 * FreeRTOS
 ******************************************/
// TODO: check if needed.
void vApplicationTickHook(void) {}

void vApplicationMallocFailedHook(void) {
    taskDISABLE_INTERRUPTS();
    PLT_UNREACHABLE;
}

// TODO check if needed.
void vApplicationIdleHook(void) {}

/**
 * @brief  Stack overflow hook // TODO: implement
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    /* Check pcTaskName for the name of the offending task,
     * or pxCurrentTCB if pcTaskName has itself been corrupted. */
    (void)xTask;
    (void)pcTaskName;
    PLT_UNREACHABLE;
}

/** @} (end addtogroup Main)   */
/** @} (end addtogroup App)    */
