// TODO modify header
/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_it.h"
#include "main.h"
#include "portmacro.h"
#include "serial.h"

#include "plt_assert.h"


/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
    PLT_UNREACHABLE;
}

// https://interrupt.memfault.com/blog/cortex-m-hardfault-debug
/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
    printf("--------------------------\n");
    printf("Hard Fault\n");
    printf("--------------------------\n");
    printf("CFSR: %08lX\n", *(uint32_t *)0xE000ED28);
    printf("UFSR: %04X\n", *(uint16_t *)0xE000ED2A);
    printf("BFSR: %02X\n", *(uint8_t *)0xE000ED29);
    printf("ABFSR: %08lX\n", *(uint32_t *)0xE000EFA8);
    printf("MMFSR: %02X\n", *(uint8_t *)0xE000ED28);
    printf("HFSR: %08lX\n", *(uint32_t *)0xE000ED2C);
    printf("Test: %08lX\n", (uint32_t)0x00000001);

    PLT_UNREACHABLE;
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
    PLT_UNREACHABLE;
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void) {
    PLT_UNREACHABLE;
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
    PLT_UNREACHABLE;
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {
    PLT_UNREACHABLE;
}

/**
 * @brief PendSV_Handler, SysTick_Handler and SVC_Handler are handled by FreeRTOS, check
 *        FreeRTOSConfig.h
 */
#if 0
/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
    PLT_UNREACHABLE;
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
    PLT_UNREACHABLE;
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
    PLT_UNREACHABLE;
}
#endif

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles UART4 global interrupt.
 */
void UART4_IRQHandler(void) {
    Serial_IrqHandler();
}
