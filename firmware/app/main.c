/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "string.h"
#include "task.h"

#include "plt_assert.h"

#include "bsp.h"
#include "rc.h"


/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/

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
