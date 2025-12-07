/**
 * @file  encoder.h
 * @brief Driver for the motor encoders.
 *
 * @ingroup   MotionEncoder
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: DriveDir_
 */

#include "plt_assert.h"
#include "plt_types.h"

#include "target.h"

#include "encoder.h"


/** @addtogroup Motion
 *    @{
 */

/** @addtogroup Encoder
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

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
 * @brief  
 *
 * @param  inp 
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. 
 */
bool_t Enc_Init(void) {
    // TODO for now just init only one of the timers
    ErrorStatus err;
    LL_APB1_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

    // Enable timer
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
    LL_TIM_ENCODER_InitTypeDef enc_cfg = {
        .EncoderMode =
            LL_TIM_ENCODERMODE_X2_TI1, /* Count on both edges LL_TIM_ENCODERMODE_X4_TI12  TODO set back */
        .IC1Polarity = LL_TIM_IC_POLARITY_RISING,      /* Polarity for input 1 */
        .IC1ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI, /* ICx is mapped on TIx */
        .IC1Prescaler = LL_TIM_ICPSC_DIV1,
        .IC1Filter = LL_TIM_IC_FILTER_FDIV1,
        .IC2Polarity = LL_TIM_IC_POLARITY_RISING,      /* Polarity for input 1 */
        .IC2ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI, /* ICx is mapped on TIx */
        .IC2Prescaler = LL_TIM_ICPSC_DIV1,
        .IC2Filter = LL_TIM_IC_FILTER_FDIV1,
    };
    LL_TIM_ENCODER_Init(TIM1, &enc_cfg);

    LL_TIM_InitTypeDef tim_cfg = {
        .Prescaler = 0,
        .CounterMode = LL_TIM_COUNTERMODE_UP,
        .Autoreload = 65535, /* Set to the max allowed by the timer */
        .ClockDivision = LL_TIM_CLOCKDIVISION_DIV1,
        .RepetitionCounter = 0,
    };
    err = LL_TIM_Init(TIM1, &tim_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    LL_TIM_DisableARRPreload(TIM1);
    LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM1);

    /* Channel GPIOs */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_InitTypeDef gpio_cfg = {
        .Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = LL_GPIO_AF_1,
    };
    err = LL_GPIO_Init(GPIOA, &gpio_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    return DEF_TRUE;
}

void Enc_StartEncoder(void) {
    LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);
    TIM1->CR1 |= TIM_CR1_CEN;
}

uint32_t Enc_GetCount(void) {
    return LL_TIM_GetCounter(TIM1);
}

void Enc_ResetCount(void) {
    TIM1->CNT = 0;
}

ENC_DIRECTION_T Enc_GetDirection(void) {
    return LL_TIM_GetDirection(TIM1) == 1U ? ENC_DIRECTION_UP : ENC_DIRECTION_DOWN;
}


/** @} (end addtogroup Encoder)  */
/** @} (end addtogroup Motion)          */
