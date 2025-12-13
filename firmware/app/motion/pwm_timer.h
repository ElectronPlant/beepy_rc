/**
 * @file      pwm_timer.h
 * @brief     Generic PWM timer.
 *
 * @ingroup   PwmTimer
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PWM_TIMER_H__
#define __PWM_TIMER_H__

#include "plt_types.h"

/********************************************************************************
 * Defines
 ********************************************************************************/
#define PWM_TIM_MAX_DUTY_CYCLE (100.0f)
#define PWM_TIM_MIN_DUTY_CYCLE (0.0f)

#define PWM_TIM_MAX_N_CHANNELS         (PWM_TIM_PORT_MAX_NUM_CHANNELS)
#define PWM_TIM_CHANNELS_BITFIELD_TYPE uint8_t
#if PWM_TIM_MAX_N_CHANNELS > 8U
    #error "PWM_TIMER_H: PWM_TIM_CHANNELS_BITFIELD_TYPE needs to be increased."
#endif

/********************************************************************************
 * Typedefs
 ********************************************************************************/

typedef enum PWM_TIM_STATUS_E {
    PWM_TIM_STATUS_UNINITIALIZED = 0,
    PWM_TIM_STATUS_INITIALIZED,
    PWM_TIM_STATUS_RUNNING,
} PWM_TIM_STATUS_T;

typedef enum {
    PWM_TIM_CHANNELS_CH1 = 0,
    PWM_TIM_CHANNELS_CH2,
    PWM_TIM_CHANNELS_CH3,
    PWM_TIM_CHANNELS_CH4,
} PWM_TIM_CHANNELS_T;

typedef const PWM_TIM_PORT_T* PWM_TIM_PERIPHERAL_T;

typedef struct PWM_TIM_INSTANCE_S {
    PWM_TIM_STATUS_T Status; /* Must be set to ENC_STATUS_UNINITIALIZED on the struct def */
    PWM_TIM_CHANNELS_BITFIELD_TYPE InitChannels; /* Bitfield for the initialized channels */
    PWM_TIM_CHANNELS_BITFIELD_TYPE EnChannels;   /* Bitfield for the started channels */
    const PWM_TIM_PERIPHERAL_T     Peripheral;
} PWM_TIM_INSTANCE_T;

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * API
 ********************************************************************************/
/* By channel */
bool_t PwmTim_InitChn(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn, uint32_t freq_kzh);
void   PwmTim_StartChn(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn);
void   PwmTim_StopChn(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn);

/* By Timer */
bool_t PwmTim_InitAll(PWM_TIM_INSTANCE_T* p_pwm, uint32_t freq_khz);
void   PwmTim_StartAll(PWM_TIM_INSTANCE_T* p_pwm);
void   PwmTim_StopAll(PWM_TIM_INSTANCE_T* p_pwm);

/* Common */
void PwmTim_ChangeFreq(PWM_TIM_INSTANCE_T* p_pwm, uint16_t freq_khz);
void PwmTim_SetDuty(PWM_TIM_INSTANCE_T* p_pwm, PWM_TIM_CHANNELS_T chn, float32_t duty);

#endif /* __PWM_TIM_H__       */
