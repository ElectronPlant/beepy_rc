/**
 * @file      pwm_timer.h
 * @brief     Generic PWM timer.
 *
 * @ingroup   PwmTimer
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PWM_TIMER_H__
#define __PWM_TIMER_H__

#include "plt_types.h"


/** @addtogroup Port
 *    @{
 */

/** @addtogroup PwmTimer
 *    @{
 */

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
    const PWM_TIM_PERIPHERAL_T     Peripheral_Ptr;
} PWM_TIM_INSTANCE_T;

typedef PWM_TIM_INSTANCE_T* PWM_TIM_HANDLER_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * API
 ********************************************************************************/

/**
  * @brief The PWM timer can be controlled in two different ways:
  *     @li By channel: PWM timers may have multiple channels. Each channel may be controlled
  *         independently. However, the PWM timer frequency is shared for all the timer channels.
  *         With the channel-by-channel interface, the timer peripheral is managed automatically.
  *         Initializing/starting it when the first channel is initialized/started; and stopped
  *         once the last channel is stopped.
  *
  *     @li By timer: The timer peripheral with all its channels may be controlled at the same time.
  */

/******************************************
 * Control By Channel
 ******************************************/

/**
 * @brief  Starts the specified PWM timer channel. See notes 1, 2.
 *
 * @param  pwm PWM timer handler.
 * @param  chn Channel to be initialized.
 * @param  freq_khz Timer frequency to set if the timer is initialized.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise. It will also return false if the
 *         requested freq_kzh value does not match the currently configured on for the already
 *         initialized timer.
 *
 * @note List of notes:
 *       1. The channel can only the initialized from the uninitialized state.
 *       2. If is the first channel to be initialized, it will also initialize the PWM timer peripheral.
 */
bool_t PwmTim_InitChn(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn, float32_t freq_kzh);

/**
 * @brief  Starts a PWM timer channel. See notes 1, 2.
 *
 * @param  pwm PWM timer handler.
 * @param  chn Channel to start.
 *
 * @note List of notes:
 *      1. The PWM timer channel must only be started if it is initialized and not already enabled.
 *      2. If this is the first channel to be started, the timer peripheral will be initialized.
 */
void PwmTim_StartChn(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn);

/**
 * @brief  Stops the PWM timer channel. See note 1 and 2.
 *
 * @param  pwm PWM timer handler.
 * @param  chn Channel to start.
 *
 * @note List of notes:
 *       1. The channel must only be enabled.
 *       2. If no other channel is enabled, the timer peripheral will be stopped.
 */
void PwmTim_StopChn(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn);


/******************************************
 * Control By Timer
 ******************************************/

/**
 * @brief  Initializes the PWM timer and all the timer channels. See note 1.
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
bool_t PwmTim_InitAll(PWM_TIM_HANDLER_T pwm, float32_t freq_khz);

/**
 * @brief  Starts the PWM timer and all its channels. See note 1.
 *
 * @param  pwm PWM timer handler.
 *
 * @note List of notes:
 *      1. The PWM timer must only be started from the initialized state.
 */
void PwmTim_StartAll(PWM_TIM_HANDLER_T pwm);

/**
 * @brief  Stops the PWM timer and all its channels. See note 1.
 *
 * @param  pwm PWM timer handler.
 *
 * @note List of notes:
 *       1. The timer must only be stopped from the running state.
 */
void PwmTim_StopAll(PWM_TIM_HANDLER_T pwm);


/******************************************
 * Common Interfaces
 ******************************************/

/**
 * @brief  Changes the timer frequency. Warning, this will change the frequency for all the timer
 *         channels.
 *
 * @param  pwm PWM timer handler.
 * @param  freq_khz New frequency to set in kHz.
 */
void PwmTim_ChangeFreq(PWM_TIM_HANDLER_T pwm, float32_t freq_khz);

/**
 * @brief  Updates the PWM duty cycle of the selected channel.
 *
 * @param  pwm  PWM timer handler.
 * @param  chn  Channel to update.
 * @param  duty Duty cycle as a percentage [PWM_TIM_MIN_DUTY_CYCLE, PWM_TIM_MAX_DUTY_CYCLE].
 */
void PwmTim_SetDuty(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn, float32_t duty);

/**
 * @brief  Updates the frequency setting the duty cycle to 50%.
 *
 * @param  pwm  PWM timer handler.
 * @param  chn  Channel to update.
 * @param  freq_khz New frequency to set in kHz.
 */
void PwmTim_SetTone(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn, float32_t freq_khz);

/**
 * @brief  Disables the PWM setting the duty cycle to zero.
 *
 * @param  pwm  PWM timer handler.
 * @param  chn  Channel to update.
 */
void PwmTim_StopTone(PWM_TIM_HANDLER_T pwm, PWM_TIM_CHANNELS_T chn);


/** @} (end addtogroup Port)        */
/** @} (end addtogroup PwmTimer)    */

#endif /* __PWM_TIM_H__       */
