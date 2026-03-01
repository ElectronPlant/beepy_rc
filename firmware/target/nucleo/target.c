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
 * Rx Serial
 ******************************************/
#include "serial_port.h"

/** Rx Serial interface.
 *  The serial interface can be used by the SBUS (only Rx with external inverter).
 *  Alternatively it can be used for debug interface.
 *
 *  The Rx serial is mapped to UART4.
 */
const SERIAL_PERIPHERAL_PORT_T TargetRcSerial = {
    .Serial = UART4,
    .Clk = LL_APB1_GRP1_PERIPH_UART4,
    .ClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,

    .IrqType = UART4_IRQn,

    .TxAvailable = DEF_TRUE,
    .TxPin = LL_GPIO_PIN_10,
    .TxPort = GPIOC,
    .TxAlternateFunc = LL_GPIO_AF_8,
    .TxClk = LL_AHB1_GRP1_PERIPH_GPIOC,
    .TxClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,

    .RxAvailable = DEF_TRUE,
    .RxPin = LL_GPIO_PIN_11,
    .RxPort = GPIOC,
    .RxAlternateFunc = LL_GPIO_AF_8,
    .RxClk = LL_AHB1_GRP1_PERIPH_GPIOC,
    .RxClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
};

/******************************************
 * Motor PWM
 ******************************************/
#include "pwm_timer_port.h"

/** Motor timer 1
 *  Is mapped to timer 2.
 *  All channels except for the third channel are used. Channel 3 is used for the
 *  I2C bus.
 *
 * @note List of notes:
 *      1. TIM2 is connected the APB2 clock, which is set to 84MHz, and it is a 32-bit timer.
 *         The PWM signal will be between 1kHz to 100kHz. To have the maximum resolution possible,
 *         the prescaller needs to be set so the count required to achieve the minimum frequency
 *         just fits the maximum count value. In this case CEIL(84MHz / (F_MIN * 2^32)) - 1 = X.
 *         With X being the prescaller, and the -1 is a correction since 0 is the identity
 *         prescaller instead of 1. In this case solves to X = 0.
 *         Note that this is for the edge aligned mode in center mode the frequency is halved.
 */
#define TARGET_MOTOR_TIM1_N_CHANNELS    (3U)
#define TARGET_MOTOR_TIM1_PRESCALLER    (0U) /* See note 1 */
#define TARGET_MOTOR_TIM1_CLK_FREQUENCY (84000 / (1 + TARGET_MOTOR_TIM1_PRESCALLER))

const PWM_TIM_PORT_CHN_T TargetMotorTim1Ch1 = {
    .GpioPin = LL_GPIO_PIN_8,
    .GpioPort = GPIOB,
    .GpioAlternateFunc = LL_GPIO_AF_1,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOB,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_CHN_T TargetMotorTim1Ch2 = {
    .GpioPin = LL_GPIO_PIN_9,
    .GpioPort = GPIOB,
    .GpioAlternateFunc = LL_GPIO_AF_1,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOB,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

/** Channel 3 is used for the I2C bus. */

const PWM_TIM_PORT_CHN_T TargetMotorTim1Ch4 = {
    .GpioPin = LL_GPIO_PIN_2,
    .GpioPort = GPIOB,
    .GpioAlternateFunc = LL_GPIO_AF_1,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOB,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_T TargetMotorTim1 = {
    .Timer = TIM2,
    .TimerClk = LL_APB1_GRP1_PERIPH_TIM2,
    .TimerClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,
    .TimerPrescaller = TARGET_MOTOR_TIM1_PRESCALLER,
    .TimerClkDivision = LL_TIM_CLOCKDIVISION_DIV1,
    .TimerClkFreqKhz = TARGET_MOTOR_TIM1_CLK_FREQUENCY,
    .TimerIs32bits = DEF_TRUE,
    .NumChannels = TARGET_MOTOR_TIM1_N_CHANNELS,
    .Channels = {&TargetMotorTim1Ch1, &TargetMotorTim1Ch2, NULL, &TargetMotorTim1Ch4}
};

/** Motor timer 2
 *  Is mapped to timer 8.
 *  This timer has all channels available.
 *
 * @note List of notes:
 *      1. TIM2 is connected the APB2 clock, which is set to 84MHz, and it is a 16-bit timer.
 *         The PWM signal will be between 1kHz to 100kHz. To have the maximum resolution possible,
 *         the prescaller needs to be set so the count required to achieve the minimum frequency
 *         just fits the maximum count value. In this case CEIL(84MHz / (F_MIN * 2^16)) - 1 = X.
 *         With X being the prescaller, and the -1 is a correction since 0 is the identity
 *         prescaller instead of 1. In this case solves to X = 0.
 *         Note that this is for the edge aligned mode in center mode the frequency is halved.
 */
#define TARGET_MOTOR_TIM2_N_CHANNELS    (4U)
#define TARGET_MOTOR_TIM2_PRESCALLER    (1U) /* See note 1 */
#define TARGET_MOTOR_TIM2_CLK_FREQUENCY (84000 / (1 + TARGET_MOTOR_TIM2_PRESCALLER))

const PWM_TIM_PORT_CHN_T TargetMotorTim2Ch1 = {
    .GpioPin = LL_GPIO_PIN_6,
    .GpioPort = GPIOC,
    .GpioAlternateFunc = LL_GPIO_AF_3,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOC,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_CHN_T TargetMotorTim2Ch2 = {
    .GpioPin = LL_GPIO_PIN_7,
    .GpioPort = GPIOC,
    .GpioAlternateFunc = LL_GPIO_AF_3,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOC,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_CHN_T TargetMotorTim2Ch3 = {
    .GpioPin = LL_GPIO_PIN_8,
    .GpioPort = GPIOC,
    .GpioAlternateFunc = LL_GPIO_AF_3,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOC,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_CHN_T TargetMotorTim2Ch4 = {
    .GpioPin = LL_GPIO_PIN_9,
    .GpioPort = GPIOC,
    .GpioAlternateFunc = LL_GPIO_AF_3,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOC,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_T TargetMotorTim2 = {
    .Timer = TIM8,
    .TimerClk = LL_APB2_GRP1_PERIPH_TIM8,
    .TimerClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,
    .TimerPrescaller = TARGET_MOTOR_TIM2_PRESCALLER,
    .TimerClkDivision = LL_TIM_CLOCKDIVISION_DIV1,
    .TimerClkFreqKhz = TARGET_MOTOR_TIM2_CLK_FREQUENCY,
    .TimerIs32bits = DEF_FALSE,
    .NumChannels = TARGET_MOTOR_TIM2_N_CHANNELS,
    .Channels = {&TargetMotorTim2Ch1, &TargetMotorTim2Ch2, &TargetMotorTim2Ch3, &TargetMotorTim2Ch4}
};

/** Motor timer 3
 *  Is mapped to timer 14.
 *  This timer only has one channel.
 *
 * @note List of notes:
 *      1. TIM14 is connected the APB1 clock, which is set to 84MHz, and it is a 16-bit timer.
 *         The PWM signal will be between 1kHz to 100kHz. To have the maximum resolution possible,
 *         the prescaller needs to be set so the count required to achieve the minimum frequency
 *         just fits the maximum count value. In this case CEIL(84MHz / (F_MIN * 2^16)) - 1 = X.
 *         With X being the prescaller, and the -1 is a correction since 0 is the identity
 *         prescaller instead of 1. In this case solves to X = 0.
 *         Note that this is for the edge aligned mode in center mode the frequency is halved.
 */
#define TARGET_MOTOR_TIM3_N_CHANNELS    (1U)
#define TARGET_MOTOR_TIM3_PRESCALLER    (1U) /* See note 1 */
#define TARGET_MOTOR_TIM3_CLK_FREQUENCY (84000 / (1 + TARGET_MOTOR_TIM3_PRESCALLER))

const PWM_TIM_PORT_CHN_T TargetMotorTim3Ch1 = {
    .GpioPin = LL_GPIO_PIN_7,
    .GpioPort = GPIOA,
    .GpioAlternateFunc = LL_GPIO_AF_9,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOA,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_T TargetMotorTim3 = {
    .Timer = TIM14,
    .TimerClk = LL_APB1_GRP1_PERIPH_TIM14,
    .TimerClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,
    .TimerPrescaller = TARGET_MOTOR_TIM3_PRESCALLER,
    .TimerClkDivision = LL_TIM_CLOCKDIVISION_DIV1,
    .TimerClkFreqKhz = TARGET_MOTOR_TIM3_CLK_FREQUENCY,
    .TimerIs32bits = DEF_FALSE,
    .NumChannels = TARGET_MOTOR_TIM3_N_CHANNELS,
    .Channels = {&TargetMotorTim3Ch1, NULL, NULL, NULL}
};

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


/******************************************
 * SERVOS
 ******************************************/
#include "pwm_timer_port.h"
/** Servo timer 1
 *  Is mapped to timer 12.
 *  Both channels of the timer are used as servo outputs.
 *
 * @note List of notes:
 *      1. TIM12 is connected the APB1 clock, which is set to 84MHz, and it is a 16-bit timer.
 *         The PWM signal will be 50Hz. To have the maximum resolution possible,
 *         the prescaller needs to be set so the count required to achieve the minimum frequency
 *         just fits the maximum count value. In this case CEIL(84MHz / (F * 2^16)) - 1 = X.
 *         With X being the prescaller, and the -1 is a correction since 0 is the identity
 *         prescaller instead of 1. In this case solves to X = 0.
 *         Note that this is for the edge aligned mode in center mode the frequency is halved.
 *         In this case the prescaller is set to 25.
 */
#define TARGET_SERVO_TIM1_N_CHANNELS    (2U)
#define TARGET_SERVO_TIM1_PRESCALLER    (25U) /* See note 1 */
#define TARGET_SERVO_TIM1_CLK_FREQUENCY (84000 / (1 + TARGET_SERVO_TIM1_PRESCALLER))

const PWM_TIM_PORT_CHN_T TargetServoTim1Ch1 = {
    .GpioPin = LL_GPIO_PIN_14,
    .GpioPort = GPIOB,
    .GpioAlternateFunc = LL_GPIO_AF_9,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOB,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_CHN_T TargetServoTim1Ch2 = {
    .GpioPin = LL_GPIO_PIN_15,
    .GpioPort = GPIOB,
    .GpioAlternateFunc = LL_GPIO_AF_9,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOB,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_T TargetServoTim1 = {
    .Timer = TIM12,
    .TimerClk = LL_APB1_GRP1_PERIPH_TIM12,
    .TimerClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,
    .TimerPrescaller = TARGET_SERVO_TIM1_PRESCALLER,
    .TimerClkDivision = LL_TIM_CLOCKDIVISION_DIV1,
    .TimerClkFreqKhz = TARGET_SERVO_TIM1_CLK_FREQUENCY,
    .TimerIs32bits = DEF_FALSE,
    .NumChannels = TARGET_SERVO_TIM1_N_CHANNELS,
    .Channels = {&TargetServoTim1Ch1, &TargetServoTim1Ch2, NULL, NULL}
};

/** Servo timer 2
 *  Is mapped to timer 13.
 *  This timer only has one channel.
 *
 * @note List of notes:
 *      1. TIM13 is connected the APB1 clock, which is set to 84MHz, and it is a 16-bit timer.
 *         The PWM signal will be 50Hz. To have the maximum resolution possible,
 *         the prescaller needs to be set so the count required to achieve the minimum frequency
 *         just fits the maximum count value. In this case CEIL(84MHz / (F * 2^16)) - 1 = X.
 *         With X being the prescaller, and the -1 is a correction since 0 is the identity
 *         prescaller instead of 1. In this case solves to X = 0.
 *         Note that this is for the edge aligned mode in center mode the frequency is halved.
 *         In this case the prescaller is set to 25.
 */
#define TARGET_SERVO_TIM2_N_CHANNELS    (1U)
#define TARGET_SERVO_TIM2_PRESCALLER    (25U) /* See note 1 */
#define TARGET_SERVO_TIM2_CLK_FREQUENCY (84000 / (1 + TARGET_SERVO_TIM1_PRESCALLER))

const PWM_TIM_PORT_CHN_T TargetServoTim2Ch1 = {
    .GpioPin = LL_GPIO_PIN_6,
    .GpioPort = GPIOA,
    .GpioAlternateFunc = LL_GPIO_AF_9,
    .GpioClk = LL_AHB1_GRP1_PERIPH_GPIOA,
    .GpioClkEnFn_Ptr = LL_AHB1_GRP1_EnableClock,
    .OutputPolarity = LL_TIM_OCPOLARITY_HIGH,
    .OutputIdleState = LL_TIM_OCIDLESTATE_LOW,
};

const PWM_TIM_PORT_T TargetServoTim2 = {
    .Timer = TIM13,
    .TimerClk = LL_APB1_GRP1_PERIPH_TIM13,
    .TimerClkEnFn_Ptr = LL_APB1_GRP1_EnableClock,
    .TimerPrescaller = TARGET_SERVO_TIM2_PRESCALLER,
    .TimerClkDivision = LL_TIM_CLOCKDIVISION_DIV1,
    .TimerClkFreqKhz = TARGET_SERVO_TIM2_CLK_FREQUENCY,
    .TimerIs32bits = DEF_FALSE,
    .NumChannels = TARGET_SERVO_TIM2_N_CHANNELS,
    .Channels = {&TargetServoTim1Ch1, NULL, NULL, NULL}
};