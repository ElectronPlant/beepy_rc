/**
 * @file  buzzer.h
 * @brief Driver for buzzers.
 *        Driver for the buzzer to play simple sounds. The buffer must not be directly driven from
 *        the MCU's GPIOs. Instead use a transistor to drive enough current through the buzzer.
 *
 * @ingroup   Buzzer
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "plt_types.h"

#include "pwm_timer.h"


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/

typedef struct BUZZER_S {
    PWM_TIM_HANDLER_T  Timer;
    PWM_TIM_CHANNELS_T Chn;
} BUZZER_T;

typedef BUZZER_T* BUZZER_HANDLER_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t Buzz_Init(BUZZER_HANDLER_T buzz);
void   Buzz_Start(BUZZER_HANDLER_T buzz);
void   Buzz_Stop(BUZZER_HANDLER_T buzz);

void Buzz_PlayTone(BUZZER_HANDLER_T buzz, float32_t freq_hz);
void Buzz_StopTone(BUZZER_HANDLER_T buzz);

#endif /* __BUZZER_H__       */
