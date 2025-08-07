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
#include "stm32f4xx_hal_cortex.h"
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

/** Drive PWM timer
 *
 * @note List of notes:
 *      1. TIMER 1 is connected the APB2 clock, which is set to 84MHz, and it is a 32-bit timer.
 *         The PWM signal will be between 1kHz to 100kHz. To have the maximum resolution possible,
 *         the prescaller needs to be set so the count required to achieve the minimum frequency
 *         just fits the maximum count value. In this case CEIL(84MHz / (F_MIN * 2^32)) - 1 = X.
 *         With X being the prescaller, and the -1 is a correction since 0 is the identity
 *         prescaller instead of 1. In this case solves to X = 0.
 *         Note that this is for the edge aligned mode in center mode the frequency is halved.
 *      2. Some timers are 16-bit timers and others are 32-bit timers. If the used timer is a 16-bit
 *         timer, set the symbol to 1; otherwise set it to 0.
 */
#define TARGET_DRIVE_PWM_TIMER              TIM2
#define TARGET_DRIVE_PWM_TIMER_CLOCK_ENABLE LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2)
#define TARGET_DRIVE_PWM_PRESCALLER         (0U) /* See note 1 */
#define TARGET_DRIVE_PWM_COUNTER_MODE       LL_TIM_COUNTERMODE_UP
#define TARGET_DRIVE_PWM_CLK_DIVISION       LL_TIM_CLOCKDIVISION_DIV1
#define TARGET_DRIVE_PWM_REP_COUNTER        (0U)
#define TARGET_DRIVE_PWM_CLK_FREQ_KHZ       (84000 / (1 + TARGET_DRIVE_PWM_PRESCALLER))
#define TARGET_DRIVE_PWM_TIMER_IS_32_BITS   (1U) /* See note 2 */

// TODO check this configs with board.
#define TARGET_DRIVE_PWM_OUTPUT_POLARITY   LL_TIM_OCPOLARITY_HIGH
#define TARGET_DRIVE_PWM_OUTPUT_IDLE_STATE LL_TIM_OCIDLESTATE_LOW

/* Pins */
#define TARGET_DRIVE_PWM_CHN_1_PIN  LL_GPIO_PIN_8
#define TARGET_DRIVE_PWM_CHN_2_PIN  LL_GPIO_PIN_9
#define TARGET_DRIVE_PWM_CHN_3_PIN  LL_GPIO_PIN_10
#define TARGET_DRIVE_PWM_CHN_4_PIN  LL_GPIO_PIN_2
#define TARGET_DRIVE_PWM_PORT       GPIOB
#define TARGET_DRIVE_PWM_GPIO_CLOCK LL_AHB1_GRP1_PERIPH_GPIOB
#define TARGET_DRIVE_PWM_GPIO_AF    LL_GPIO_AF_1

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
