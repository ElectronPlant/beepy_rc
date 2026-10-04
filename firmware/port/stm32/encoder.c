/**
 * @file  encoder.h
 * @brief STM32 port of the motor encoder driver.
 *
 * @ingroup   EncoderStm32
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Enc_
 */

#include "plt_assert.h"
#include "plt_types.h"

#include "encoder_port.h"
#include "target.h"

#include "encoder.h"


/** @addtogroup Port
 *   @{
 */

/** @addtogroup Encoder
 *   @{
 */

/** @addtogroup EncoderStm32
 *   @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
/**
 * Running encoder asserts are set in all the functions that are not init, start
 * or stop. This macro control if the state of the encoder is checked when the
 * other functions are called (when set to 1) or not (when set to 0).
 */
#define ENC_ENABLE_RUNNING_ASSERT 1

#if ENC_ENABLE_RUNNING_ASSERT == 1
    #define ENC_RUNNING_ASSERT(enc) PLT_ASSERT(ENC_STATUS_RUNNING == enc->Status)
#else
    #define ENC_RUNNING_ASSERT(enc)
#endif /* ENC_ENABLE_RUNNING_ASSERT == 1 */

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  STM32 port for the encoder init function.
 */
bool_t Enc_Init(ENC_HANDLER_T enc) {
    ErrorStatus err;

    PLT_ASSERT(NULL != enc);
    PLT_ASSERT(ENC_STATUS_UNINITIALIZED == enc->Status);

    /* Initialize the encoder timer */
    enc->Peripheral->TimerClkEnFn_Ptr(enc->Peripheral->TimerClk);
    LL_TIM_ENCODER_InitTypeDef enc_cfg = {
        .EncoderMode = LL_TIM_ENCODERMODE_X4_TI12,     /* Count on both edges */
        .IC1Polarity = LL_TIM_IC_POLARITY_RISING,      /* Polarity for input 1 */
        .IC1ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI, /* ICx is mapped on TIx */
        .IC1Prescaler = LL_TIM_ICPSC_DIV1,
        .IC1Filter = LL_TIM_IC_FILTER_FDIV1,
        .IC2Polarity = LL_TIM_IC_POLARITY_RISING,      /* Polarity for input 1 */
        .IC2ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI, /* ICx is mapped on TIx */
        .IC2Prescaler = LL_TIM_ICPSC_DIV1,
        .IC2Filter = LL_TIM_IC_FILTER_FDIV1,
    };
    LL_TIM_ENCODER_Init(enc->Peripheral->Timer, &enc_cfg);

    LL_TIM_InitTypeDef tim_cfg = {
        .Prescaler = 0,
        .CounterMode = LL_TIM_COUNTERMODE_UP,
        .Autoreload = 65535, /* Set to the max allowed by the timer */
        .ClockDivision = LL_TIM_CLOCKDIVISION_DIV1,
        .RepetitionCounter = 0,
    };
    err = LL_TIM_Init(enc->Peripheral->Timer, &tim_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    LL_TIM_DisableARRPreload(enc->Peripheral->Timer);
    LL_TIM_SetTriggerOutput(enc->Peripheral->Timer, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(enc->Peripheral->Timer);

    /* Initialize GPIO 1 */
    enc->Peripheral->Gpio1ClkEnFn_Ptr(enc->Peripheral->Gpio1Clk);
    LL_GPIO_InitTypeDef gpio1_cfg = {
        .Pin = enc->Peripheral->Gpio1Pin,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = enc->Peripheral->Gpio1AlternateFunc,
    };
    err = LL_GPIO_Init(enc->Peripheral->Gpio1Port, &gpio1_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    /* Initialize GPIO 2 */
    enc->Peripheral->Gpio2ClkEnFn_Ptr(enc->Peripheral->Gpio2Clk);
    LL_GPIO_InitTypeDef gpio2_cfg = {
        .Pin = enc->Peripheral->Gpio2Pin,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = enc->Peripheral->Gpio2AlternateFunc,
    };
    err = LL_GPIO_Init(enc->Peripheral->Gpio2Port, &gpio2_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    enc->Status = ENC_STATUS_INITIALIZED;
    return DEF_TRUE;
}

/**
 * @brief  STM32 port of the encoder start function.
 */
void Enc_StartEncoder(ENC_HANDLER_T enc) {
    PLT_ASSERT(NULL != enc);
    PLT_ASSERT(ENC_STATUS_INITIALIZED == enc->Status);

    LL_TIM_CC_DisableChannel(
        enc->Peripheral->Timer,
        enc->Peripheral->TimerChn1 | enc->Peripheral->TimerChn2
    );
    LL_TIM_CC_EnableChannel(
        enc->Peripheral->Timer,
        enc->Peripheral->TimerChn1 | enc->Peripheral->TimerChn2
    );
    enc->Peripheral->Timer->CR1 |= TIM_CR1_CEN;

    enc->Peripheral->Timer->CNT = 0;
    enc->Status = ENC_STATUS_RUNNING;
}

/**
 * @brief  STM32 port of the stop encoder function.
 */
void Enc_StopEncoder(ENC_HANDLER_T enc) {
    PLT_ASSERT(NULL != enc);
    PLT_ASSERT(ENC_STATUS_RUNNING == enc->Status);

    LL_TIM_CC_DisableChannel(
        enc->Peripheral->Timer,
        enc->Peripheral->TimerChn1 | enc->Peripheral->TimerChn2
    );
    enc->Peripheral->Timer->CR1 &= ~TIM_CR1_CEN;

    enc->Status = ENC_STATUS_INITIALIZED;
}

/**
 * @brief  STM32 port for the get encoder count function.
 */
uint32_t Enc_GetCount(ENC_HANDLER_T enc) {
    PLT_ASSERT(NULL != enc);
    ENC_RUNNING_ASSERT(enc);
    return LL_TIM_GetCounter(enc->Peripheral->Timer);
}

/**
 * @brief  STM32 port for the reset encoder count function.
 */
void Enc_ResetCount(ENC_HANDLER_T enc) {
    ENC_RUNNING_ASSERT(enc);
    enc->Peripheral->Timer->CNT = 0;
}

/**
 * @brief  STM32 port for the get encoder direction function.
 */
ENC_DIRECTION_T Enc_GetDirection(ENC_HANDLER_T enc) {
    ENC_RUNNING_ASSERT(enc);
    return LL_TIM_GetDirection(enc->Peripheral->Timer) == 1U ? ENC_DIRECTION_UP
                                                             : ENC_DIRECTION_DOWN;
}


/** @} (end addtogroup EncoderStm32) */
/** @} (end addtogroup Encoder)      */
/** @} (end addtogroup Port)         */
