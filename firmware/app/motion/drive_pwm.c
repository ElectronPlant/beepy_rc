/**
 * @file      drive_pwm.c
 * @brief     Drive Motor PWM implementation.
 *
 * @ingroup   DrivePwm
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note Module Prefix: DrivePwm_
 *
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "target.h"


#include "drive_pwm.h"


/** @addtogroup Motion
 *   @{
 */

/** @addtogroup DrivePwm
 *   @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#if TARGET_DRIVE_PWM_TIMER_IS_32_BITS == 1U
    #define DRIVE_PWM_MAX_TIMER_COUNT (UINT32_MAX)
#else
    #define DRIVE_PWM_MAX_TIMER_COUNT (UINT16_MAX)
#endif

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static uint32_t DrivePwm_GetAutoReload(uint32_t freq_khz);
static uint32_t DrivePwm_Duty2CompareValue(float32_t duty_cycle);

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/
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
static uint32_t DrivePwm_GetAutoReload(uint32_t freq_khz) {
    float32_t clk_freq = TARGET_DRIVE_PWM_CLK_FREQ_KHZ;
    float32_t reload_f = (clk_freq / (float32_t)freq_khz);

    /* See note 1 */
    PLT_ASSERT((float32_t)DRIVE_PWM_MAX_TIMER_COUNT >= reload_f && 0.0f <= reload_f);
    uint32_t reload = PLT_UTILS_ROUND_FLOAT_TO_UINT(reload_f);

    return reload;
}

/**
 * @brief  Initializes the drive PWM.
 *
 *  @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t DrivePwm_Init(void) {
    ErrorStatus err;
    uint32_t    autoreload = DrivePwm_GetAutoReload(50);

    TARGET_DRIVE_PWM_TIMER_CLOCK_ENABLE;

    LL_TIM_InitTypeDef tim_cfg = {
        .Prescaler = TARGET_DRIVE_PWM_PRESCALLER,
        .CounterMode = TARGET_DRIVE_PWM_COUNTER_MODE,
        .Autoreload = autoreload,
        .ClockDivision = TARGET_DRIVE_PWM_CLK_DIVISION,
        .RepetitionCounter = TARGET_DRIVE_PWM_REP_COUNTER,
    };
    err = LL_TIM_Init(TARGET_DRIVE_PWM_TIMER, &tim_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_DisableARRPreload(TARGET_DRIVE_PWM_TIMER);
    LL_TIM_SetClockSource(TARGET_DRIVE_PWM_TIMER, LL_TIM_CLOCKSOURCE_INTERNAL);

    /* Enable channels */
    LL_TIM_OC_InitTypeDef channel_cfg = {
        .OCMode = LL_TIM_OCMODE_PWM1,
        .OCState = LL_TIM_OCSTATE_DISABLE,
        .CompareValue = 0,
        .OCPolarity = TARGET_DRIVE_PWM_OUTPUT_POLARITY,
        .OCNPolarity = TARGET_DRIVE_PWM_OUTPUT_POLARITY,
        .OCIdleState = TARGET_DRIVE_PWM_OUTPUT_IDLE_STATE,
        .OCNIdleState = TARGET_DRIVE_PWM_OUTPUT_IDLE_STATE
    };
    LL_TIM_OC_EnablePreload(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH1);
    err = LL_TIM_OC_Init(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH1, &channel_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_OC_DisableFast(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH1);

    LL_TIM_OC_EnablePreload(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH2);
    err = LL_TIM_OC_Init(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH2, &channel_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_OC_DisableFast(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH2);

    LL_TIM_OC_EnablePreload(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH3);
    err = LL_TIM_OC_Init(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH3, &channel_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_OC_DisableFast(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH3);

    LL_TIM_OC_EnablePreload(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH4);
    err = LL_TIM_OC_Init(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH4, &channel_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }
    LL_TIM_OC_DisableFast(TARGET_DRIVE_PWM_TIMER, LL_TIM_CHANNEL_CH4);

    /* Timer cfg */
    LL_TIM_SetTriggerOutput(TARGET_DRIVE_PWM_TIMER, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TARGET_DRIVE_PWM_TIMER);

    /* Channel GPIOs */
    LL_AHB1_GRP1_EnableClock(TARGET_DRIVE_PWM_GPIO_CLOCK);
    LL_GPIO_InitTypeDef gpio_cfg = {
        .Pin = TARGET_DRIVE_PWM_CHN_1_PIN | TARGET_DRIVE_PWM_CHN_2_PIN | TARGET_DRIVE_PWM_CHN_3_PIN
            | TARGET_DRIVE_PWM_CHN_4_PIN,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = TARGET_DRIVE_PWM_GPIO_AF,
    };
    err = LL_GPIO_Init(TARGET_DRIVE_PWM_PORT, &gpio_cfg);
    if (SUCCESS != err) {
        return DEF_FALSE;
    }

    return DEF_TRUE;
}

/**
 * @brief  Starts the PWM timer.
 */
void DrivePwm_Start(void) {
    /* Enable Timer */
    TARGET_DRIVE_PWM_TIMER->CR1 |= TIM_CR1_CEN;

    /* Start all channels with duty cycle set to zero */
    // for (uint8_t ch = DRIVE_PWM_CHANNELS_CH1; ch < DRIVE_PWM_CHANNELS_CH4; ch++) {
    //     DrivePwm_SetDuty(ch, 0.0f);
    // }
    TARGET_DRIVE_PWM_TIMER->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
}

// TODO this function is just used for the motor test.
void DrivePwm_ChangeFreq(uint16_t freq_khz) {
    uint16_t autoreload = DrivePwm_GetAutoReload(freq_khz);
    LL_TIM_SetAutoReload(TARGET_DRIVE_PWM_TIMER, autoreload);
}

/**
 * @brief  Computes the Output Compare value for a channel to achieve the desired duty cycle.
 *
 * @param  duty_cycle Desired duty cycle.
 *
 * @return Calculated output compare value.
 */
static uint32_t DrivePwm_Duty2CompareValue(float32_t duty_cycle) {
    PLT_ASSERT(DRIVE_PWM_MAX_DUTY_CYCLE >= duty_cycle && DRIVE_PWM_MIN_DUTY_CYCLE <= duty_cycle);

    uint32_t  reload_value = TARGET_DRIVE_PWM_TIMER->ARR;
    float32_t f_compare = (float32_t)reload_value;
    f_compare /= (DRIVE_PWM_MAX_DUTY_CYCLE - DRIVE_PWM_MIN_DUTY_CYCLE);
    f_compare *= (duty_cycle - DRIVE_PWM_MIN_DUTY_CYCLE);

    return (uint32_t)PLT_UTILS_ROUND_FLOAT_TO_UINT(f_compare);
}

/**
 * @brief  Updates the PWM duty cycle of the selected channel.
 *
 * @param  chn  Channel to update.
 * @param  duty Duty cycle as a percentage [DRIVE_PWM_MIN_DUTY_CYCLE, DRIVE_PWM_MAX_DUTY_CYCLE].
 */
void DrivePwm_SetDuty(DRIVE_PWM_CHANNELS_T chn, float32_t duty) {
    uint32_t compare_value = DrivePwm_Duty2CompareValue(duty);
    printf("Compare value: %lu\n", compare_value);

    switch (chn) {
        case DRIVE_PWM_CHANNELS_CH1:
            TARGET_DRIVE_PWM_TIMER->CCR1 = compare_value;
            break;
        case DRIVE_PWM_CHANNELS_CH2:
            TARGET_DRIVE_PWM_TIMER->CCR2 = compare_value;
            break;
        case DRIVE_PWM_CHANNELS_CH3:
            TARGET_DRIVE_PWM_TIMER->CCR3 = compare_value;
            break;
        case DRIVE_PWM_CHANNELS_CH4:
            TARGET_DRIVE_PWM_TIMER->CCR4 = compare_value;
            break;
        default:
            PLT_UNREACHABLE;
    };
}

/** @} (end addtogroup DrivePwm)   */
/** @} (end addtogroup Motion)     */
