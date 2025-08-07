/**
 * @file      drive_pwm.h
 * @brief     Drive Motor PWM implementation.
 *
 * @ingroup   DrivePwm
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __DRIVE_PWM_H__
#define __DRIVE_PWM_H__

#include "plt_types.h"

/********************************************************************************
 * Defines
 ********************************************************************************/
#define DRIVE_PWM_MAX_DUTY_CYCLE (100.0f)
#define DRIVE_PWM_MIN_DUTY_CYCLE (0.0f)

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum {
    DRIVE_PWM_CHANNELS_CH1 = 0,
    DRIVE_PWM_CHANNELS_CH2,
    DRIVE_PWM_CHANNELS_CH3,
    DRIVE_PWM_CHANNELS_CH4
} DRIVE_PWM_CHANNELS_T;

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * API
 ********************************************************************************/
bool_t DrivePwm_Init(void);
void   DrivePwm_Start(void);

void DrivePwm_ChangeFreq(uint16_t freq_khz); // TODO just for tests
void DrivePwm_SetDuty(DRIVE_PWM_CHANNELS_T chn, float32_t duty);

#endif /* __DRIVE_PWM_H__       */
