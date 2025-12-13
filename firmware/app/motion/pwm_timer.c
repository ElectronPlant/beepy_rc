/**
 * @file      pwm_timer.c
 * @brief     Drive Motor PWM implementation.
 *
 * @ingroup   PwmTim
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note Module Prefix: PwmTim_
 *
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "target.h"


#include "pwm_timer.h"


/** @addtogroup Motion
 *   @{
 */

/** @addtogroup PwmTimer
 *   @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
/**
 * Running PWM timer asserts are set in all the functions that are not init, start
 * or stop. This macro control if the state of the encoder is checked when the
 * other functions are called (when set to 1) or not (when set to 0).
 */
#define PWM_TIM_ENABLE_RUNNING_ASSERT 1

#if PWM_TIM_ENABLE_RUNNING_ASSERT == 1
    #define PWM_TIM_RUNNING_ASSERT(p_pwm) PLT_ASSERT(PWM_TIM_STATUS_RUNNING == p_pwm->Status)
#else
    #define PWM_TIM_RUNNING_ASSERT(p_enc)
#endif /* PWM_TIM_ENABLE_RUNNING_ASSERT == 1 */

/* --- Channel bitfields --- */
#define PWM_TIM_GET_CHN_BITFIELD_MSK(CHN) (0x01 << CHN)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static inline uint32_t PwmTim_GetMaxTimerCnt(PWM_TIM_INSTANCE_T* p_pwm);
static inline bool_t   PwmTim_IsAnyChnEnabled(PWM_TIM_INSTANCE_T* p_pwm);
static inline bool_t   PwmTim_IsChnEnabled(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn);

static uint32_t PwmTim_GetAutoReload(PWM_TIM_INSTANCE_T* p_pwm, uint32_t freq_khz);
static uint32_t PwmTim_Duty2CompareValue(PWM_TIM_INSTANCE_T* p_pwm, float32_t duty_cycle);

static void PwmTim_StartTimer(PWM_TIM_INSTANCE_T* p_pwm);
static void PwmTim_StartChnInternal(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn);
static void PwmTim_StopTimer(PWM_TIM_INSTANCE_T* p_pwm);
static void PwmTim_StopChnInternal(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
static const uint32_t PwmTim_ChnNumToLLChn[PWM_TIM_MAX_N_CHANNELS] =
    {LL_TIM_CHANNEL_CH1, LL_TIM_CHANNEL_CH2, LL_TIM_CHANNEL_CH3, LL_TIM_CHANNEL_CH4};

static const uint32_t PwmTim_ChnEnableBits[PWM_TIM_MAX_N_CHANNELS] =
    {TIM_CCER_CC1E, TIM_CCER_CC2E, TIM_CCER_CC3E, TIM_CCER_CC4E};

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Gets the maximum timer count.
 *
 * @param  p_pwm: Pointer to the PWM timer instance.
 *
 * @return Max timer CNT.
 */
static inline uint32_t PwmTim_GetMaxTimerCnt(PWM_TIM_INSTANCE_T* p_pwm) {
    return DEF_TRUE == p_pwm->Peripheral->TimerIs32bits ? UINT32_MAX : UINT16_MAX;
}

/**
 * @brief  Checks if any of the timer channel is initialized.
 *
 * @param  p_pwm: Pointer to the PWM timer instance to check.
 *
 * @return DEF_TRUE if at least one of the timer channels is initialized; DEF_FALSE otherwise.
 */
static inline bool_t PwmTim_IsAnyChnInit(PWM_TIM_INSTANCE_T* p_pwm) {
    return 0x00 == p_pwm->InitChannels ? DEF_FALSE : DEF_TRUE;
}

/**
 * @brief  Checks if the specified PWM timer channel is initialized.
 *
 * @param  p_pwm: Pointer to the PWM timer instance of the channel to check.
 * @param chn: Timer channel to check. It may be multiple channels OR'd together.
 *
 * @return DEF_TRUE if the channel is initialized; DEF_FALSE otherwise.
 */
static inline bool_t PwmTim_IsChnInit(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn) {
    return 0x00 == (p_pwm->InitChannels & PWM_TIM_GET_CHN_BITFIELD_MSK(chn)) ? DEF_FALSE : DEF_TRUE;
}

/**
 * @brief  Checks if any of the timer channel is enabled.
 *
 * @param  p_pwm: Pointer to the PWM timer instance to check.
 *
 * @return DEF_TRUE if at least one of the timer channels is enabled; DEF_FALSE otherwise.
 */
static inline bool_t PwmTim_IsAnyChnEnabled(PWM_TIM_INSTANCE_T* p_pwm) {
    return 0x00 == p_pwm->EnChannels ? DEF_FALSE : DEF_TRUE;
}

/**
 * @brief  Checks if the specified PWM timer channel is enabled.
 *
 * @param  p_pwm: Pointer to the PWM timer instance of the channel to check.
 * @param chn: Timer channel to check. It may be multiple channels OR'd together.
 *
 * @return DEF_TRUE if the channel is enabled; DEF_FALSE otherwise.
 */
static inline bool_t PwmTim_IsChnEnabled(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn) {
    return 0x00 == (p_pwm->EnChannels & PWM_TIM_GET_CHN_BITFIELD_MSK(chn)) ? DEF_FALSE : DEF_TRUE;
}

/**
 * @brief Computes the timer's auto-reload value for the timer to achieve the desired frequency.
 *
 * @param  freq_khz Desired frequency in kHz.
 *
 * @return Auto-reload value.
 *
 * @note List of notes:
 *      1. If the prescaller is not correctly configured, the auto-reload value may not be usable.
 *         If the prescaller is too low, the count may not be high-enough for the required
 *         frequency. Otherwise, if the prescaller is to high, there may not be enough resolution
 *         to set the required frequency.
 */
static uint32_t PwmTim_GetAutoReload(PWM_TIM_INSTANCE_T* p_pwm, uint32_t freq_khz) {
    float32_t clk_freq = p_pwm->Peripheral->TimerClkFreqKhz;
    float32_t reload_f = (clk_freq / (float32_t)freq_khz);

    /* See note 1 */
    PLT_ASSERT((float32_t)PwmTim_GetMaxTimerCnt(p_pwm) >= reload_f && 0.0f <= reload_f);
    uint32_t reload = PLT_UTILS_ROUND_FLOAT_TO_UINT(reload_f);

    return reload;
}

/**
 * @brief  Initializes the specified channel of the PWM timer. See note 1.
 *
 * @param  p_pwm: Pointer to the PWM timer instance of the channel to be initialized.
 * @param  chn_num: Number of the PWM to initialize.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 * 
 * @note List of notes:
 *      1. The channel must not be already initialized.
 */
static bool_t PwmTim_InitChnInternal(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn) {
    const PWM_TIM_PORT_CHN_T* p_chn = p_pwm->Peripheral->Channels[chn];
    ErrorStatus               err;

    PLT_ASSERT(NULL != p_chn);
    PLT_ASSERT(PWM_TIM_MAX_N_CHANNELS >= chn);
    PLT_ASSERT(DEF_FALSE == PwmTim_IsChnInit(p_pwm, chn));
    PLT_ASSERT(DEF_FALSE == PwmTim_IsChnEnabled(p_pwm, chn));

    /* Timer channel */
    LL_TIM_OC_InitTypeDef channel_cfg = {
        .OCMode = LL_TIM_OCMODE_PWM1,
        .OCState = LL_TIM_OCSTATE_DISABLE,
        .CompareValue = 0,
        .OCPolarity = p_chn->OutputPolarity,
        .OCNPolarity = p_chn->OutputPolarity,
        .OCIdleState = p_chn->OutputIdleState,
        .OCNIdleState = p_chn->OutputIdleState
    };
    LL_TIM_OC_EnablePreload(p_pwm->Peripheral->Timer, PwmTim_ChnNumToLLChn[chn]);
    err = LL_TIM_OC_Init(p_pwm->Peripheral->Timer, PwmTim_ChnNumToLLChn[chn], &channel_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_OC_DisableFast(p_pwm->Peripheral->Timer, PwmTim_ChnNumToLLChn[chn]);

    /* Channel GPIO */
    p_chn->GpioClkEnFn_Ptr(p_chn->GpioClk);
    LL_GPIO_InitTypeDef gpio_cfg = {
        .Pin = p_chn->GpioPin,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = p_chn->GpioAlternateFunc,
    };
    err = LL_GPIO_Init(p_chn->GpioPort, &gpio_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    /* Set channel enabled bit */
    p_pwm->InitChannels |= PWM_TIM_GET_CHN_BITFIELD_MSK(chn);

    return DEF_TRUE;
}

/**
 * @brief  Initializes the PWM timer. See note 1.
 *
 * @param p_pwm: Pointer to the PWM timer instance to init.
 * @param freq_khz: Timer frequency to set in kHz.
 *
 *  @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *      1. The timer must only be initialized from the uninitialized state.
 *      2. The repetition counter is used to generate interrupts after a given number
 *         of periods. This value is set to zero by default, since this functionality is not
 *         supported for the moment.
 */
static bool_t PwmTim_InitTimer(PWM_TIM_INSTANCE_T* p_pwm, uint32_t freq_khz) {
    ErrorStatus err;
    uint32_t    autoreload = PwmTim_GetAutoReload(p_pwm, freq_khz);

    PLT_ASSERT(PWM_TIM_STATUS_UNINITIALIZED == p_pwm->Status); /* See note 1 */
    PLT_ASSERT(DEF_FALSE == PwmTim_IsAnyChnEnabled(p_pwm));

    /* Enable timer clock */
    p_pwm->Peripheral->TimerClkEnFn_Ptr(p_pwm->Peripheral->TimerClk);

    /* Configure timer peripheral */
    LL_TIM_InitTypeDef tim_cfg = {
        .Prescaler = p_pwm->Peripheral->TimerPrescaller,
        .CounterMode = LL_TIM_COUNTERMODE_UP, /* Default value */
        .Autoreload = autoreload,
        .ClockDivision = p_pwm->Peripheral->TimerClkDivision,
        .RepetitionCounter = 0U, /* Default value, see note 2. */
    };
    err = LL_TIM_Init(p_pwm->Peripheral->Timer, &tim_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_DisableARRPreload(p_pwm->Peripheral->Timer);
    LL_TIM_SetClockSource(p_pwm->Peripheral->Timer, LL_TIM_CLOCKSOURCE_INTERNAL);

    /* Timer cfg */
    LL_TIM_SetTriggerOutput(p_pwm->Peripheral->Timer, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(p_pwm->Peripheral->Timer);

    p_pwm->Status = PWM_TIM_STATUS_INITIALIZED;
    return DEF_TRUE;
}

/**
 * @brief  Starts the specified PWM timer channel. See notes 1, 2.
 *
 * @param  p_pwm: Pointer to the PWM timer instance of the channel to init.
 * @param  chn: Channel to be initialized.
 * @param  freq_khz: Timer frequency to set if the timer is initialized.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise. It will also return false if the
 *         requested freq_kzh value does not match the currently configured on for the already
 *         initialized timer.
 *
 * @note List of notes:
 *       1. The channel can only the initialized from the uninitialized state.
 *       2. If is the first channel to be enabled, it will also enabled the PWM timer peripheral.
 */
bool_t PwmTim_InitChn(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn, uint32_t freq_khz) {
    PLT_ASSERT(NULL != p_pwm);

    /* Initialized the timer if needed, see note 2 */
    if (DEF_FALSE == PwmTim_IsAnyChnEnabled(p_pwm)) {
        bool_t ok = PwmTim_InitTimer(p_pwm, freq_khz);
        if (DEF_FALSE == ok) {
            return DEF_FALSE;
        }
    }

    return PwmTim_InitChnInternal(p_pwm, chn);
}

/**
 * @brief  Initializes the PWM timer and all the timer channels. See note 1.
 *
 * @param p_pwm: Pointer to the PWM timer instance to init.
 * @param freq_khz: Timer frequency to set in kHz.
 *
 *  @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *      1. The timer must only be initialized from the uninitialized state.
 *      2. The repetition counter is used to generate interrupts after a given number
 *         of periods. This value is set to zero by default, since this functionality is not
 *         supported for the moment.
 */
bool_t PwmTim_InitAll(PWM_TIM_INSTANCE_T* p_pwm, uint32_t freq_khz) {
    PLT_ASSERT(PWM_TIM_STATUS_UNINITIALIZED == p_pwm->Status); /* See note 1 */

    bool_t ok = PwmTim_InitTimer(p_pwm, freq_khz);
    if (DEF_FALSE == ok) {
        return DEF_FALSE;
    }

    /* Enable channels */
    uint8_t rem_chn = p_pwm->Peripheral->NumChannels;
    for (uint8_t chn = 0; chn < PWM_TIM_MAX_N_CHANNELS && rem_chn > 0; chn++) {
        if (NULL != p_pwm->Peripheral->Channels[chn]) {
            bool_t chn_ok = PwmTim_InitChnInternal(p_pwm, chn);
            if (DEF_FALSE == chn_ok) {
                return DEF_FALSE;
            }
            rem_chn--;
        }
    }
    PLT_ASSERT(0 == rem_chn); /* All channels must be initialized */
    return DEF_TRUE;
}

/**
 * @brief  Starts the PWM timer peripheral. See note 1.
 *
 * @param  p_pwm: Pointer to the PWM timer peripheral.
 *
 * @note list of notes:
 *      1. The PWM timer must only be started from the initialized state.
 */
static void PwmTim_StartTimer(PWM_TIM_INSTANCE_T* p_pwm) {
    PLT_ASSERT(PWM_TIM_STATUS_INITIALIZED == p_pwm->Status); /* See note 1 */

    /* Enable Timer */
    p_pwm->Peripheral->Timer->CR1 |= TIM_CR1_CEN;
    p_pwm->Status = PWM_TIM_STATUS_RUNNING;
}

/**
 * @brief  Generic function to start a specific timer channel. See note 1.
 *
 * @param  p_pwm: Pointer to the PWM timer peripheral.
 * @param chn: Channel to start.
 *
 * @note List of notes:
 *      1. A timer channel must only be started if is wasn't already enabled.
 */
static void PwmTim_StartChnInternal(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn) {
    PLT_ASSERT(NULL != p_pwm->Peripheral->Channels[chn]);
    PLT_ASSERT(DEF_TRUE == PwmTim_IsChnInit(p_pwm, chn));
    PLT_ASSERT(DEF_FALSE == PwmTim_IsChnEnabled(p_pwm, chn)); /* See note 1 */

    PwmTim_SetDuty(p_pwm, chn, 0.0f);
    p_pwm->Peripheral->Timer->CCER |= PwmTim_ChnEnableBits[chn];
    p_pwm->EnChannels |= PWM_TIM_GET_CHN_BITFIELD_MSK(chn);
}

/**
 * @brief  Stop the PWM timer peripheral. See note 1.
 *
 * @param  p_pwm: Pointer to the PWM timer peripheral.
 *
 * @note list of notes:
 *      1. The PWM timer must only be Stopped if it is already enabled.
 */
static void PwmTim_StopTimer(PWM_TIM_INSTANCE_T* p_pwm) {
    PLT_ASSERT(PWM_TIM_STATUS_RUNNING == p_pwm->Status); /* See note 1 */

    /* Enable Timer */
    p_pwm->Peripheral->Timer->CR1 &= ~TIM_CR1_CEN;
    p_pwm->Status = PWM_TIM_STATUS_INITIALIZED;
}

/**
 * @brief  Generic function to stop a specific timer channel. See note 1.
 *
 * @param  p_pwm: Pointer to the PWM timer peripheral.
 * @param chn: Channel to start.
 *
 * @param note List of notes:
 *      1. A timer channel must only be stopped if is was already enabled.
 */
static void PwmTim_StopChnInternal(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn) {
    PLT_ASSERT(NULL != p_pwm->Peripheral->Channels[chn]);
    PLT_ASSERT(DEF_TRUE == PwmTim_IsChnInit(p_pwm, chn));
    PLT_ASSERT(DEF_TRUE == PwmTim_IsChnEnabled(p_pwm, chn)); /* See note 1 */

    p_pwm->Peripheral->Timer->CCER &= ~PwmTim_ChnEnableBits[chn];
    p_pwm->EnChannels &= ~PWM_TIM_GET_CHN_BITFIELD_MSK(chn);
}

/**
 * @brief  Starts the PWM timer and all its channels. See note 1.
 *
 * @note List of notes:
 *      1. The PWM timer channel must only be started if not already enabled.
 */
void PwmTim_StartChn(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn) {
    PLT_ASSERT(NULL != p_pwm);

    /* Start timer if required */
    if (DEF_FALSE == PwmTim_IsAnyChnEnabled(p_pwm)) {
        PwmTim_StartTimer(p_pwm);
    }

    /* Start channel */
    PwmTim_StartChnInternal(p_pwm, chn);
}

/**
 * @brief  Starts the PWM timer and all its channels. See note 1.
 *
 * @note List of notes:
 *      1. The PWM timer must only be started from the initialized state.
 */
void PwmTim_StartAll(PWM_TIM_INSTANCE_T* p_pwm) {
    PLT_ASSERT(NULL != p_pwm); /* See note 1 */

    /* Enable Timer */
    PwmTim_StartTimer(p_pwm);

    /* Start all channels with duty cycle set to zero */
    uint8_t rem_chn = p_pwm->Peripheral->NumChannels;
    for (uint8_t chn = 0; chn < PWM_TIM_MAX_N_CHANNELS && rem_chn > 0; chn++) {
        if (NULL != p_pwm->Peripheral->Channels[chn]) {
            PwmTim_StartChnInternal(p_pwm, chn);
            rem_chn--;
        }
    }
    PLT_ASSERT(0 == rem_chn); /* All channels must be initialized */
}

/**
 * @brief  Stops the PWM timer channel. See note 1 and 2.
 *
 * @param  p_pwm: Pointer to the PWM timer instance to stop.
 *
 * @note List of notes:
 *       1. The channel must only be enabled.
 *       2. If no other channel is enabled, the timer peripheral will be stopped.
 */
void PwmTim_StopChn(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn) {
    PLT_ASSERT(NULL == p_pwm);

    /* Disable Timer if needed, see note 2 */
    if (DEF_FALSE == PwmTim_IsAnyChnEnabled(p_pwm)) {
        PwmTim_StopChnInternal(p_pwm, chn);
    }

    /* Stop all channels */
    PwmTim_InitChnInternal(p_pwm, chn);
}

/**
 * @brief  Stops the PWM timer and all its channels. See note 1.
 *
 * @param  p_pwm: Pointer to the PWM timer instance to stop.
 *
 * @note List of notes:
 *       1. The timer must only be stopped from the running state.
 */
void PwmTim_Stop(PWM_TIM_INSTANCE_T* p_pwm) {
    PLT_ASSERT(NULL != p_pwm);

    /* Disable Timer */
    PwmTim_StopTimer(p_pwm);

    /* Stop all channels */
    uint8_t rem_chn = p_pwm->Peripheral->NumChannels;
    for (uint8_t chn = 0; chn < PWM_TIM_MAX_N_CHANNELS && rem_chn > 0; chn++) {
        if (NULL != p_pwm->Peripheral->Channels[chn]) {
            PwmTim_StopChnInternal(p_pwm, chn);
            rem_chn--;
        }
    }
    PLT_ASSERT(0 == rem_chn); /* All channels must be initialized */
}

/**
 * @brief  Changes the timer frequency. Warning, this will change the frequency for all the timer
 *         channels.
 *
 * @param  p_pwm: POinter to the PWM timer instance to which the frequency will be changed.
 * @param  freq_khz: New frequency to set in kHz.
 */
void PwmTim_ChangeFreq(PWM_TIM_INSTANCE_T* p_pwm, uint16_t freq_khz) {
    PLT_ASSERT(NULL != p_pwm);
    PWM_TIM_RUNNING_ASSERT(p_pwm);
    uint16_t autoreload = PwmTim_GetAutoReload(p_pwm, freq_khz);
    LL_TIM_SetAutoReload(p_pwm->Peripheral->Timer, autoreload);
}

/**
 * @brief  Computes the Output Compare value for a channel to achieve the desired duty cycle.
 *
 * @param  duty_cycle Desired duty cycle.
 *
 * @return Calculated output compare value.
 */
static uint32_t PwmTim_Duty2CompareValue(PWM_TIM_INSTANCE_T* p_pwm, float32_t duty_cycle) {
    PLT_ASSERT(NULL != p_pwm);
    PWM_TIM_RUNNING_ASSERT(p_pwm);
    PLT_ASSERT(PWM_TIM_MAX_DUTY_CYCLE >= duty_cycle && PWM_TIM_MIN_DUTY_CYCLE <= duty_cycle);

    uint32_t  reload_value = p_pwm->Peripheral->Timer->ARR;
    float32_t f_compare = (float32_t)reload_value;
    f_compare /= (PWM_TIM_MAX_DUTY_CYCLE - PWM_TIM_MIN_DUTY_CYCLE);
    f_compare *= (duty_cycle - PWM_TIM_MIN_DUTY_CYCLE);

    return (uint32_t)PLT_UTILS_ROUND_FLOAT_TO_UINT(f_compare);
}

/**
 * @brief  Updates the PWM duty cycle of the selected channel.
 *
 * @param  chn  Channel to update.
 * @param  duty Duty cycle as a percentage [PWM_TIM_MIN_DUTY_CYCLE, PWM_TIM_MAX_DUTY_CYCLE].
 */
void PwmTim_SetDuty(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn, float32_t duty) {
    PLT_ASSERT(NULL != p_pwm);
    PWM_TIM_RUNNING_ASSERT(p_pwm);

    uint32_t compare_value = PwmTim_Duty2CompareValue(p_pwm, duty);

    switch (chn) {
        case PWM_TIM_CHANNELS_CH1:
            p_pwm->Peripheral->Timer->CCR1 = compare_value;
            break;
        case PWM_TIM_CHANNELS_CH2:
            p_pwm->Peripheral->Timer->CCR2 = compare_value;
            break;
        case PWM_TIM_CHANNELS_CH3:
            p_pwm->Peripheral->Timer->CCR3 = compare_value;
            break;
        case PWM_TIM_CHANNELS_CH4:
            p_pwm->Peripheral->Timer->CCR4 = compare_value;
            break;
        default:
            PLT_UNREACHABLE;
    };
}

/** @} (end addtogroup PwmTimer)   */
/** @} (end addtogroup Motion)     */
