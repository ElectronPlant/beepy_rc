/**
 * @file      target_nucleo.h
 * @brief     target definition for the nucleo-F446 board.
 *
 * @ingroup   target_nucleo
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __TARGET_H__
#define __TARGET_H__

/* Includes for the HAL */
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_utils.h"

/********************************************************************************
 * Defines
 ********************************************************************************/
/******************************************
 * Internal Components
 ******************************************/
/* --- LED --- */
#define TARGET_USE_LED
#define TARGET_LED_PIN        LL_GPIO_PIN_5
#define TARGET_LED_PORT       GPIOA
#define TARGET_LED_GPIO_CLOCK LL_AHB1_GRP1_PERIPH_GPIOA

/* --- Button --- */
/** @note List of notes:
 *      1. EXTI ints can be seen here:
 *         https://controllerstech.com/external-interrupt-using-registers/
 */
#define TARGET_USE_BUTTON
#define TARGET_BUTTON_PIN              LL_GPIO_PIN_13
#define TARGET_BUTTON_PORT             GPIOC
#define TARGET_BUTTON_GPIO_CLOCK       LL_AHB1_GRP1_PERIPH_GPIOC
#define TARGET_BUTTON_EXTI_LINE        LL_EXTI_LINE_13
#define TARGET_BUTTON_EXTI_IRQ         EXTI15_10_IRQn /* Note 1 */
#define TARGET_BUTTON_EXIT_IRQ_HANDLER EXTI15_10_IRQHandler
#define TARGET_BUTTON_SYSCFG_EXTI_PORT LL_SYSCFG_EXTI_PORTC
#define TARGET_BUTTON_SYSCFG_EXTI_LINE LL_SYSCFG_EXTI_LINE13

/* --- */ // TODO things I'm not sure I need
#if 0
#define USART_TX_Pin       LL_GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin       LL_GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define TMS_Pin            LL_GPIO_PIN_13
#define TMS_GPIO_Port      GPIOA
#define TCK_Pin            LL_GPIO_PIN_14
#define TCK_GPIO_Port      GPIOA
#define SWO_Pin            LL_GPIO_PIN_3
#define SWO_GPIO_Port      GPIOB
#endif

/******************************************
 * External Components
 ******************************************/
/* UART port used for RC */
#define TARGET_RC_SERIAL_PERIPH_CLOCK LL_APB1_GRP1_PERIPH_UART4
#define TARGET_RC_SERIAL_GPIO_CLOCK   LL_AHB1_GRP1_PERIPH_GPIOC
#define TARGET_RC_SERIAL_INSTANCE     UART4
#define TARGET_RC_SERIAL_IRQ          UART4_IRQn
#define TARGET_RC_SERIAL_IRQ_HANDLER  UART4_IRQHandler
#define TARGET_RC_SERIAL_TX_PIN       LL_GPIO_PIN_10
#define TARGET_RC_SERIAL_RX_PIN       LL_GPIO_PIN_11
#define TARGET_RC_SERIAL_PORT         GPIOC

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * API
 ********************************************************************************/


#endif /* __TARGET_H__       */
