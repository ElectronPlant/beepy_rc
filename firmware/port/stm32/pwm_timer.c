/**
 * @file      pwm_timer.c
 * @brief     STM32 port for the PWM timer driver.
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


/** @addtogroup PortsStm32
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
    #define PWM_TIM_RUNNING_ASSERT(pwm) PLT_ASSERT(PWM_TIM_STATUS_RUNNING == pwm->Status)
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
static inline uint32_t PwmTim_GetMaxTimerCnt(PWM_TIM_HANDLER_T pwm);
static inline bool_t   PwmTim_IsAnyChnEnabled(PWM_TIM_HANDLER_T pwm);
static inline bool_t   PwmTim_IsChnEnabled(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn);

static uint32_t PwmTim_GetAutoReload(PWM_TIM_HANDLER_T pwm, float32_t freq_khz);
static uint32_t PwmTim_Duty2CompareValue(PWM_TIM_HANDLER_T pwm, float32_t duty_cycle);

static void PwmTim_StartTimer(PWM_TIM_HANDLER_T pwm);
static void PwmTim_StartChnInternal(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn);
static void PwmTim_StopTimer(PWM_TIM_HANDLER_T pwm);
static void PwmTim_StopChnInternal(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn);


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

/******************************************
 * Utils
 ******************************************/
/**
 * @brief  Gets the maximum timer count.
 *
 * @param  pwm PWM timer handler.
 *
 * @return Max timer CNT.
 */
static inline uint32_t PwmTim_GetMaxTimerCnt(PWM_TIM_HANDLER_T pwm) {
    return DEF_TRUE == pwm->Peripheral_Ptr->TimerIs32bits ? UINT32_MAX : UINT16_MAX;
}

/**
 * @brief  Checks if any of the timer channel is initialized.
 *
 * @param  pwm PWM timer handler.
 *
 * @return DEF_TRUE if at least one of the timer channels is initialized; DEF_FALSE otherwise.
 */
static inline bool_t PwmTim_IsAnyChnInit(PWM_TIM_HANDLER_T pwm) {
    return 0x00 == pwm->InitChannels ? DEF_FALSE : DEF_TRUE;
}

/**
 * @brief  Checks if the specified PWM timer channel is initialized.
 *
 * @param  pwm PWM timer handler.
 * @param  chn Timer channel to check. It may be multiple channels OR'd together.
 *
 * @return DEF_TRUE if the channel is initialized; DEF_FALSE otherwise.
 */
static inline bool_t PwmTim_IsChnInit(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn) {
    return 0x00 == (pwm->InitChannels & PWM_TIM_GET_CHN_BITFIELD_MSK(chn)) ? DEF_FALSE : DEF_TRUE;
}

/**
 * @brief  Checks if any of the timer channel is enabled.
 *
 * @param  pwm PWM timer handler.
 *
 * @return DEF_TRUE if at least one of the timer channels is enabled; DEF_FALSE otherwise.
 */
static inline bool_t PwmTim_IsAnyChnEnabled(PWM_TIM_HANDLER_T pwm) {
    return 0x00 == pwm->EnChannels ? DEF_FALSE : DEF_TRUE;
}

/**
 * @brief  Checks if the specified PWM timer channel is enabled.
 *
 * @param  pwm PWM timer handler.
 * @param  chn Timer channel to check. It may be multiple channels OR'd together.
 *
 * @return DEF_TRUE if the channel is enabled; DEF_FALSE otherwise.
 */
static inline bool_t PwmTim_IsChnEnabled(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn) {
    return 0x00 == (pwm->EnChannels & PWM_TIM_GET_CHN_BITFIELD_MSK(chn)) ? DEF_FALSE : DEF_TRUE;
}

/******************************************
 * Main
 ******************************************/
/**
 * @brief Computes the timer's auto-reload value for the timer to achieve the desired frequency.
 *
 * @param  pwm PWM timer handler.
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
static uint32_t PwmTim_GetAutoReload(PWM_TIM_HANDLER_T pwm, float32_t freq_khz) {
    float32_t clk_freq = pwm->Peripheral_Ptr->TimerClkFreqKhz;
    float32_t reload_f = (clk_freq / freq_khz);

    /* See note 1 */
    PLT_ASSERT((float32_t)PwmTim_GetMaxTimerCnt(pwm) >= reload_f && 0.0f <= reload_f);
    uint32_t reload = PLT_UTILS_ROUND_FLOAT_TO_UINT(reload_f);

    return reload;
}

/**
 * @brief  Initializes the specified channel of the PWM timer. See note 1.
 *
 * @param  pwm PWM timer handler.
 * @param  chn PWM timer channel to initialize.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *      1. The channel must not be already initialized.
 */
static bool_t PwmTim_InitChnInternal(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn) {
    const PWM_TIM_PORT_CHN_T* p_chn = pwm->Peripheral_Ptr->Channels[chn];
    ErrorStatus               err;

    PLT_ASSERT(NULL != p_chn);
    PLT_ASSERT(PWM_TIM_MAX_N_CHANNELS >= chn);
    PLT_ASSERT(DEF_FALSE == PwmTim_IsChnInit(pwm, chn));
    PLT_ASSERT(DEF_FALSE == PwmTim_IsChnEnabled(pwm, chn));

    /* Timer channel */
    LL_TIM_OC_InitTypeDef channel_cfg = {
        .OCMode = LL_TIM_OCMODE_PWM1,
        .OCState = LL_TIM_OCSTATE_DISABLE,
        .OCNState = LL_TIM_OCSTATE_DISABLE,
        .CompareValue = 0,
        .OCPolarity = p_chn->OutputPolarity,
        .OCNPolarity = p_chn->OutputPolarity,
        .OCIdleState = p_chn->OutputIdleState,
        .OCNIdleState = p_chn->OutputIdleState
    };
    LL_TIM_OC_EnablePreload(pwm->Peripheral_Ptr->Timer, PwmTim_ChnNumToLLChn[chn]);
    err = LL_TIM_OC_Init(pwm->Peripheral_Ptr->Timer, PwmTim_ChnNumToLLChn[chn], &channel_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_OC_DisableFast(pwm->Peripheral_Ptr->Timer, PwmTim_ChnNumToLLChn[chn]);

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
    pwm->InitChannels |= PWM_TIM_GET_CHN_BITFIELD_MSK(chn);

    return DEF_TRUE;
}

/**
 * @brief  Initialized the advanced timer specific fields.
 *
 * @param  pwm PWM timer handler.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
static bool_t PwmTim_InitAdvancedTimer(PWM_TIM_HANDLER_T pwm) {
    LL_TIM_BDTR_InitTypeDef adv_tim_cfg = {
        .OSSRState = LL_TIM_OSSR_DISABLE,
        .OSSIState = LL_TIM_OSSI_DISABLE,
        .LockLevel = LL_TIM_LOCKLEVEL_OFF,
        .DeadTime = 0,
        .BreakState = LL_TIM_BREAK_DISABLE,
        .BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH,
        .AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE,
    };
    ErrorStatus err = LL_TIM_BDTR_Init(pwm->Peripheral_Ptr->Timer, &adv_tim_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    return DEF_TRUE;
}

/**
 * @brief  Initializes the PWM timer. See note 1.
 *
 * @param  pwm PWM timer handler.
 * @param  freq_khz Timer frequency to set in kHz.
 *
 *  @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *      1. The timer must only be initialized from the uninitialized state.
 *      2. The repetition counter is used to generate interrupts after a given number
 *         of periods. This value is set to zero by default, since this functionality is not
 *         supported for the moment.
 */
static bool_t PwmTim_InitTimer(PWM_TIM_HANDLER_T pwm, float32_t freq_khz) {
    ErrorStatus err;
    uint32_t    autoreload;

    PLT_ASSERT(PWM_TIM_STATUS_UNINITIALIZED == pwm->Status); /* See note 1 */
    PLT_ASSERT(DEF_FALSE == PwmTim_IsAnyChnEnabled(pwm));

    autoreload = PwmTim_GetAutoReload(pwm, freq_khz);

    /* Enable timer clock */
    pwm->Peripheral_Ptr->TimerClkEnFn_Ptr(pwm->Peripheral_Ptr->TimerClk);

    /* Configure timer peripheral */
    LL_TIM_InitTypeDef tim_cfg = {
        .Prescaler = pwm->Peripheral_Ptr->TimerPrescaller,
        .CounterMode = LL_TIM_COUNTERMODE_UP, /* Default value */
        .Autoreload = autoreload,
        .ClockDivision = pwm->Peripheral_Ptr->TimerClkDivision,
        .RepetitionCounter = 0U, /* Default value, see note 2. */
    };
    err = LL_TIM_Init(pwm->Peripheral_Ptr->Timer, &tim_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_DisableARRPreload(pwm->Peripheral_Ptr->Timer);
    LL_TIM_SetClockSource(pwm->Peripheral_Ptr->Timer, LL_TIM_CLOCKSOURCE_INTERNAL);

    /* Timer cfg */
    LL_TIM_SetTriggerOutput(pwm->Peripheral_Ptr->Timer, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(pwm->Peripheral_Ptr->Timer);

    /* Advance timer cfg */
    if (DEF_TRUE == pwm->Peripheral_Ptr->TimerIsAdvanced) {
        if (DEF_FALSE == PwmTim_InitAdvancedTimer(pwm)) {
            return DEF_FALSE;
        }
    }

    pwm->Status = PWM_TIM_STATUS_INITIALIZED;
    return DEF_TRUE;
}

/**
 * @brief  STM32 port of the init PWM timer channel function.
 */
bool_t PwmTim_InitChn(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn, float32_t freq_khz) {
    PLT_ASSERT(NULL != pwm);

    /* Initialized the timer if needed, see note 2 */
    if (DEF_FALSE == PwmTim_IsAnyChnInit(pwm)) {
        bool_t ok = PwmTim_InitTimer(pwm, freq_khz);
        if (DEF_FALSE == ok) {
            return DEF_FALSE;
        }
    }

    return PwmTim_InitChnInternal(pwm, chn);
}

/**
 * @brief  STM32 port of the init PWM timer with all channels function.
 */
bool_t PwmTim_InitAll(PWM_TIM_HANDLER_T pwm, float32_t freq_khz) {
    PLT_ASSERT(PWM_TIM_STATUS_UNINITIALIZED == pwm->Status); /* See note 1 */

    bool_t ok = PwmTim_InitTimer(pwm, freq_khz);
    if (DEF_FALSE == ok) {
        return DEF_FALSE;
    }

    /* Enable channels */
    uint8_t rem_chn = pwm->Peripheral_Ptr->NumChannels;
    for (uint8_t chn = 0; chn < PWM_TIM_MAX_N_CHANNELS && rem_chn > 0; chn++) {
        if (NULL != pwm->Peripheral_Ptr->Channels[chn]) {
            bool_t chn_ok = PwmTim_InitChnInternal(pwm, chn);
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
 * @param  pwm PWM timer handler.
 *
 * @note list of notes:
 *      1. The PWM timer must only be started from the initialized state.
 */
static void PwmTim_StartTimer(PWM_TIM_HANDLER_T pwm) {
    PLT_ASSERT(PWM_TIM_STATUS_INITIALIZED == pwm->Status); /* See note 1 */

    /* Advanced timers */
    if (DEF_TRUE == pwm->Peripheral_Ptr->TimerIsAdvanced) {
        pwm->Peripheral_Ptr->Timer->BDTR |= TIM_BDTR_MOE;
    }

    /* Enable Timer */
    pwm->Peripheral_Ptr->Timer->CR1 |= TIM_CR1_CEN;
    pwm->Status = PWM_TIM_STATUS_RUNNING;
}

/**
 * @brief  Generic function to start a specific timer channel. See note 1.
 *
 * @param  pwm PWM timer handler.
 * @param  chn Channel to start.
 *
 * @note List of notes:
 *      1. The PWM timer channel must only be started if it is initialized and not already enabled.
 */
static void PwmTim_StartChnInternal(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn) {
    PLT_ASSERT(NULL != pwm->Peripheral_Ptr->Channels[chn]);
    PLT_ASSERT(DEF_TRUE == PwmTim_IsChnInit(pwm, chn));
    PLT_ASSERT(DEF_FALSE == PwmTim_IsChnEnabled(pwm, chn)); /* See note 1 */

    PwmTim_SetDuty(pwm, chn, 0.0f);
    pwm->Peripheral_Ptr->Timer->CCER |= PwmTim_ChnEnableBits[chn];
    pwm->EnChannels |= PWM_TIM_GET_CHN_BITFIELD_MSK(chn);
}

/**
 * @brief  Stop the PWM timer peripheral. See note 1.
 *
 * @param  pwm PWM timer handler.
 *
 * @note list of notes:
 *      1. The PWM timer must only be Stopped from the running state.
 */
static void PwmTim_StopTimer(PWM_TIM_HANDLER_T pwm) {
    PLT_ASSERT(PWM_TIM_STATUS_RUNNING == pwm->Status); /* See note 1 */

    /* Enable Timer */
    pwm->Peripheral_Ptr->Timer->CR1 &= ~TIM_CR1_CEN;
    pwm->Status = PWM_TIM_STATUS_INITIALIZED;
}

/**
 * @brief  Generic function to stop a specific timer channel. See note 1.
 *
 * @param  pwm PWM timer handler.
 * @param  chn Channel to start.
 *
 * @param note List of notes:
 *      1. The PWM timer channel must only be stopped if it is initialized and enabled.
 */
static void PwmTim_StopChnInternal(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn) {
    PLT_ASSERT(NULL != pwm->Peripheral_Ptr->Channels[chn]);
    PLT_ASSERT(DEF_TRUE == PwmTim_IsChnInit(pwm, chn));
    PLT_ASSERT(DEF_TRUE == PwmTim_IsChnEnabled(pwm, chn)); /* See note 1 */

    pwm->Peripheral_Ptr->Timer->CCER &= ~PwmTim_ChnEnableBits[chn];
    pwm->EnChannels &= ~PWM_TIM_GET_CHN_BITFIELD_MSK(chn);
}

/**
 * @brief  STM32 port of the start PWM timer channel function.
 */
void PwmTim_StartChn(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn) {
    PLT_ASSERT(NULL != pwm);

    /* Start timer if required */
    if (DEF_FALSE == PwmTim_IsAnyChnEnabled(pwm)) {
        PwmTim_StartTimer(pwm);
    }

    /* Start channel */
    PwmTim_StartChnInternal(pwm, chn);
}

/**
 * @brief  STM32 port for the start PWM timer with all its channels function.
 */
void PwmTim_StartAll(PWM_TIM_HANDLER_T pwm) {
    PLT_ASSERT(NULL != pwm); /* See note 1 */

    /* Enable Timer */
    PwmTim_StartTimer(pwm);

    /* Start all channels with duty cycle set to zero */
    uint8_t rem_chn = pwm->Peripheral_Ptr->NumChannels;
    for (uint8_t chn = 0; chn < PWM_TIM_MAX_N_CHANNELS && rem_chn > 0; chn++) {
        if (NULL != pwm->Peripheral_Ptr->Channels[chn]) {
            PwmTim_StartChnInternal(pwm, chn);
            rem_chn--;
        }
    }
    PLT_ASSERT(0 == rem_chn); /* All channels must be initialized */
}

/**
 * @brief  STM32 port for the stop PWM timer channel function.
 */
void PwmTim_StopChn(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn) {
    PLT_ASSERT(NULL == pwm);

    /* Disable Timer if needed, see note 2 */
    if (DEF_FALSE == PwmTim_IsAnyChnEnabled(pwm)) {
        PwmTim_StopChnInternal(pwm, chn);
    }

    /* Stop all channels */
    PwmTim_InitChnInternal(pwm, chn);
}

/**
 * @brief  STM32 port for the Stop PWM timer with all its channels function.
 */
void PwmTim_Stop(PWM_TIM_HANDLER_T pwm) {
    PLT_ASSERT(NULL != pwm);

    /* Disable Timer */
    PwmTim_StopTimer(pwm);

    /* Stop all channels */
    uint8_t rem_chn = pwm->Peripheral_Ptr->NumChannels;
    for (uint8_t chn = 0; chn < PWM_TIM_MAX_N_CHANNELS && rem_chn > 0; chn++) {
        if (NULL != pwm->Peripheral_Ptr->Channels[chn]) {
            PwmTim_StopChnInternal(pwm, chn);
            rem_chn--;
        }
    }
    PLT_ASSERT(0 == rem_chn); /* All channels must be initialized */
}

/**
 * @brief  STM32 port for the change PWM timer frequency function.
 */
void PwmTim_ChangeFreq(PWM_TIM_HANDLER_T pwm, uint16_t freq_khz) {
    PLT_ASSERT(NULL != pwm);
    PWM_TIM_RUNNING_ASSERT(pwm);
    uint16_t autoreload = PwmTim_GetAutoReload(pwm, freq_khz);
    LL_TIM_SetAutoReload(pwm->Peripheral_Ptr->Timer, autoreload);
}

/**
 * @brief  Computes the Output Compare value for a channel to achieve the desired duty cycle.
 *
 * @param  pwm PWM timer handler.
 * @param  duty_cycle Desired duty cycle.
 *
 * @return Calculated output compare value.
 */
static uint32_t PwmTim_Duty2CompareValue(PWM_TIM_HANDLER_T pwm, float32_t duty_cycle) {
    PLT_ASSERT(NULL != pwm);
    PWM_TIM_RUNNING_ASSERT(pwm);
    PLT_ASSERT(PWM_TIM_MAX_DUTY_CYCLE >= duty_cycle && PWM_TIM_MIN_DUTY_CYCLE <= duty_cycle);

    uint32_t  reload_value = pwm->Peripheral_Ptr->Timer->ARR;
    float32_t f_compare = (float32_t)reload_value;
    f_compare /= (PWM_TIM_MAX_DUTY_CYCLE - PWM_TIM_MIN_DUTY_CYCLE);
    f_compare *= (duty_cycle - PWM_TIM_MIN_DUTY_CYCLE);

    return (uint32_t)PLT_UTILS_ROUND_FLOAT_TO_UINT(f_compare);
}

/**
 * @brief  STM32 port for the set PWM duty cycle function.
 */
void PwmTim_SetDuty(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn, float32_t duty) {
    PLT_ASSERT(NULL != pwm);
    PWM_TIM_RUNNING_ASSERT(pwm);

    uint32_t compare_value = PwmTim_Duty2CompareValue(pwm, duty);

    switch (chn) {
        case PWM_TIM_CHANNELS_CH1:
            pwm->Peripheral_Ptr->Timer->CCR1 = compare_value;
            break;
        case PWM_TIM_CHANNELS_CH2:
            pwm->Peripheral_Ptr->Timer->CCR2 = compare_value;
            break;
        case PWM_TIM_CHANNELS_CH3:
            pwm->Peripheral_Ptr->Timer->CCR3 = compare_value;
            break;
        case PWM_TIM_CHANNELS_CH4:
            pwm->Peripheral_Ptr->Timer->CCR4 = compare_value;
            break;
        default:
            PLT_UNREACHABLE;
    };
}

/** @} (end addtogroup PwmTimer)   */
/** @} (end addtogroup PortsStm32)     */
