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

#include "encoder_port.h"
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
/**
 * Running encoder asserts are set in all the functions that are not init, start
 * or stop. This macro control if the state of the encoder is checked when the
 * other functions are called (when set to 1) or not (when set to 0).
 */
#define ENC_ENABLE_RUNNING_ASSERT 1

#if ENC_ENABLE_RUNNING_ASSERT == 1
    #define ENC_RUNNING_ASSERT(p_enc) PLT_ASSERT(ENC_STATUS_RUNNING == p_enc->Status)
#else
    #define ENC_RUNNING_ASSERT(p_enc)
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
 * @brief  Initializes the encoder. See note 1.
 *         Encoders are composed from a timer and two GPIO inputs for the quadrature signal.
 *
 * @param  p_enc: Pointer to the encoder instance to initialize.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. The encoder instance must be uninitialized before being uninitialized.
 */
bool_t Enc_Init(ENC_INSTANCE_T* p_enc) {
    ErrorStatus err;

    PLT_ASSERT(NULL != p_enc);
    PLT_ASSERT(ENC_STATUS_UNINITIALIZED == p_enc->Status);

    /* Initialize the encoder timer */
    p_enc->Peripheral->TimerClkEnFn_Ptr(p_enc->Peripheral->TimerClk);
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
    LL_TIM_ENCODER_Init(p_enc->Peripheral->Timer, &enc_cfg);

    LL_TIM_InitTypeDef tim_cfg = {
        .Prescaler = 0,
        .CounterMode = LL_TIM_COUNTERMODE_UP,
        .Autoreload = 65535, /* Set to the max allowed by the timer */
        .ClockDivision = LL_TIM_CLOCKDIVISION_DIV1,
        .RepetitionCounter = 0,
    };
    err = LL_TIM_Init(p_enc->Peripheral->Timer, &tim_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    LL_TIM_DisableARRPreload(p_enc->Peripheral->Timer);
    LL_TIM_SetTriggerOutput(p_enc->Peripheral->Timer, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(p_enc->Peripheral->Timer);

    /* Initialize GPIO 1 */
    p_enc->Peripheral->Gpio1ClkEnFn_Ptr(p_enc->Peripheral->Gpio1Clk);
    LL_GPIO_InitTypeDef gpio1_cfg = {
        .Pin = p_enc->Peripheral->Gpio1Pin,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = p_enc->Peripheral->Gpio1AlternateFunc,
    };
    err = LL_GPIO_Init(p_enc->Peripheral->Gpio1Port, &gpio1_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    /* Initialize GPIO 2 */
    p_enc->Peripheral->Gpio2ClkEnFn_Ptr(p_enc->Peripheral->Gpio2Clk);
    LL_GPIO_InitTypeDef gpio2_cfg = {
        .Pin = p_enc->Peripheral->Gpio2Pin,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = p_enc->Peripheral->Gpio2AlternateFunc,
    };
    err = LL_GPIO_Init(p_enc->Peripheral->Gpio2Port, &gpio2_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    p_enc->Status = ENC_STATUS_INITIALIZED;
    return DEF_TRUE;
}

/**
 * @brief  Starts the encoder count. See note 1.
 *
 * @param  p_enc: Pointer to the encoder instance to start.
 *
 * @note List of notes:
 *       1. The encoder must be initialized to be started.
 */
void Enc_StartEncoder(ENC_INSTANCE_T* p_enc) {
    PLT_ASSERT(NULL != p_enc);
    PLT_ASSERT(ENC_STATUS_INITIALIZED == p_enc->Status);

    LL_TIM_CC_DisableChannel(
        p_enc->Peripheral->Timer,
        p_enc->Peripheral->TimerChn1 | p_enc->Peripheral->TimerChn2
    );
    LL_TIM_CC_EnableChannel(
        p_enc->Peripheral->Timer,
        p_enc->Peripheral->TimerChn1 | p_enc->Peripheral->TimerChn2
    );
    p_enc->Peripheral->Timer->CR1 |= TIM_CR1_CEN;

    p_enc->Peripheral->Timer->CNT = 0;
    p_enc->Status = ENC_STATUS_RUNNING;
}

/**
 * @brief  Stops the encoder count. See note 1.
 *
 * @param  p_enc: Pointer to the encoder instance to stop.
 *
 * @note List of notes:
 *       1. The encoder must be running before it can be stopped.
 */
void Enc_StopEncoder(ENC_INSTANCE_T* p_enc) {
    PLT_ASSERT(NULL != p_enc);
    PLT_ASSERT(ENC_STATUS_RUNNING == p_enc->Status);

    LL_TIM_CC_DisableChannel(
        p_enc->Peripheral->Timer,
        p_enc->Peripheral->TimerChn1 | p_enc->Peripheral->TimerChn2
    );
    p_enc->Peripheral->Timer->CR1 &= ~TIM_CR1_CEN;

    p_enc->Status = ENC_STATUS_INITIALIZED;
}

/**
 * @brief  Reads the encoder count. See note 1.
 *
 * @param  p_enc: Pointer to the encoder instance from which to read the count.
 *
 * @return Current encoder count.
 *
 * @note List of notes:
 *       1. The encoder must be running to perform a correct count reading.
 */
uint32_t Enc_GetCount(ENC_INSTANCE_T* p_enc) {
    PLT_ASSERT(NULL != p_enc);
    ENC_RUNNING_ASSERT(p_enc);
    return LL_TIM_GetCounter(p_enc->Peripheral->Timer);
}

/**
 * @brief  Resets the encoder count to zero. See note 1.
 *
 * @param  p_enc: Pointer to the encoder instance to reset.
 *
 *  @note List of notes:
 *       1. The encoder must be running to reset the count.
 */
void Enc_ResetCount(ENC_INSTANCE_T* p_enc) {
    ENC_RUNNING_ASSERT(p_enc);
    p_enc->Peripheral->Timer->CNT = 0;
}

/**
 * @brief  Reads the count direction of the encoder. See note 1.
 *
 * @param  p_enc: Pointer to the encoder instance from which to read the direction.
 *
 * @return ENC_DIRECTION_UP if the encoder direction is counting up, ENC_DIRECTION_DOWN otherwise.
 *
 * @note List of notes:
 *       1. The encoder must be running to get a valid direction reading.
 */
ENC_DIRECTION_T Enc_GetDirection(ENC_INSTANCE_T* p_enc) {
    ENC_RUNNING_ASSERT(p_enc);
    return LL_TIM_GetDirection(p_enc->Peripheral->Timer) == 1U ? ENC_DIRECTION_UP
                                                               : ENC_DIRECTION_DOWN;
}


/** @} (end addtogroup Encoder)  */
/** @} (end addtogroup Motion)          */
