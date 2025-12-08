/**
 * @file  target.c
 * @brief Target definition for the nucleo-F446 board.
 *
 * @ingroup   Target
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Target_
 */

#include "plt_assert.h"
#include "plt_types.h"

#include "target.h"


/** @addtogroup Target
 *    @{
 */

/******************************************
 * Encoders
 ******************************************/
#include "encoder_port.h"
const ENC_PERIPHERAL_PORT_T TargetEnc1 = {
    .Timer = TIM1,
    .TimerClk = LL_APB2_GRP1_PERIPH_TIM1,
    .TimerClkEnFn_Ptr = LL_APB2_GRP1_EnableClock,
    .TimerChn1 = LL_TIM_CHANNEL_CH1,
    .TimerChn2 = LL_TIM_CHANNEL_CH2,

    .Gpio1Pin = LL_GPIO_PIN_8,
    .Gpio1Port = GPIOA,
    .Gpio1AlternateFunc = LL_GPIO_AF_1,
    .Gpio1Clk = LL_AHB1_GRP1_PERIPH_GPIOA,
    .Gpio1ClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,

    .Gpio2Pin = LL_GPIO_PIN_9,
    .Gpio2Port = GPIOA,
    .Gpio2AlternateFunc = LL_GPIO_AF_1,
    .Gpio2Clk = LL_AHB1_GRP1_PERIPH_GPIOA,
    .Gpio2ClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
};

const ENC_PERIPHERAL_PORT_T TargetEnc2 = {
    .Timer = TIM3,
    .TimerClk = LL_APB1_GRP1_PERIPH_TIM3,
    .TimerClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,
    .TimerChn1 = LL_TIM_CHANNEL_CH1,
    .TimerChn2 = LL_TIM_CHANNEL_CH2,

    .Gpio1Pin = LL_GPIO_PIN_6,
    .Gpio1Port = GPIOA,
    .Gpio1AlternateFunc = LL_GPIO_AF_2,
    .Gpio1Clk = LL_AHB1_GRP1_PERIPH_GPIOA,
    .Gpio1ClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,

    .Gpio2Pin = LL_GPIO_PIN_7,
    .Gpio2Port = GPIOA,
    .Gpio2AlternateFunc = LL_GPIO_AF_2,
    .Gpio2Clk = LL_AHB1_GRP1_PERIPH_GPIOA,
    .Gpio2ClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
};

const ENC_PERIPHERAL_PORT_T TargetEnc3 = {
    .Timer = TIM4,
    .TimerClk = LL_APB1_GRP1_PERIPH_TIM4,
    .TimerClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,
    .TimerChn1 = LL_TIM_CHANNEL_CH1,
    .TimerChn2 = LL_TIM_CHANNEL_CH2,

    .Gpio1Pin = LL_GPIO_PIN_6,
    .Gpio1Port = GPIOB,
    .Gpio1AlternateFunc = LL_GPIO_AF_2,
    .Gpio1Clk = LL_AHB1_GRP1_PERIPH_GPIOB,
    .Gpio1ClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,

    .Gpio2Pin = LL_GPIO_PIN_7,
    .Gpio2Port = GPIOB,
    .Gpio2AlternateFunc = LL_GPIO_AF_2,
    .Gpio2Clk = LL_AHB1_GRP1_PERIPH_GPIOB,
    .Gpio2ClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
};

const ENC_PERIPHERAL_PORT_T TargetEnc4 = {
    .Timer = TIM5,
    .TimerClk = LL_APB1_GRP1_PERIPH_TIM5,
    .TimerClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,
    .TimerChn1 = LL_TIM_CHANNEL_CH1,
    .TimerChn2 = LL_TIM_CHANNEL_CH2,

    .Gpio1Pin = LL_GPIO_PIN_0,
    .Gpio1Port = GPIOA,
    .Gpio1AlternateFunc = LL_GPIO_AF_2,
    .Gpio1Clk = LL_AHB1_GRP1_PERIPH_GPIOA,
    .Gpio1ClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,

    .Gpio2Pin = LL_GPIO_PIN_1,
    .Gpio2Port = GPIOA,
    .Gpio2AlternateFunc = LL_GPIO_AF_2,
    .Gpio2Clk = LL_AHB1_GRP1_PERIPH_GPIOA,
    .Gpio2ClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
};


/** @} (end addtogroup Target)  */
