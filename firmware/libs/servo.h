/**
 * @file  servo.h
 * @brief Driver for analog servo motors.
 *        Analog servos allow setting their angle based on the duty cycle of their PWM control
 *        signal. The signal has a period of 20ms (50Hz) and the duration of 1ms (for 0deg) and
 *        a duration of 2ms (for 180deg).
 *
 * @ingroup   Servo
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __SERVO_H__
#define __SERVO_H__

#include "plt_types.h"

#include "pwm_timer.h"


/********************************************************************************
 * Defines
 ********************************************************************************/
#define SERVO_MAX_PERCENTAGE (100.0f)
#define SERVO_MIN_PERCENTAGE (-100.0f)

#define SERVO_DEFAULT_INITIAL_SPAN (0.0f)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

typedef struct SERVO_S {
    PWM_TIM_HANDLER_T  Timer;
    PWM_TIM_CHANNELS_T Chn;
    float32_t          CurrentSpan;
    float32_t          MinSpan;
    float32_t          MaxSpan;
} SERVO_T;

typedef SERVO_T* SERVO_HANDLER_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t Servo_Init(SERVO_HANDLER_T servo);
void   Servo_Start(SERVO_HANDLER_T servo, float32_t initial_value);
void   Servo_Stop(SERVO_HANDLER_T Servo);

void Servo_SetAngle(SERVO_HANDLER_T Servo, float32_t angle);
void Servo_SetSpanLimits(SERVO_HANDLER_T servo, float32_t max, float32_t min);
void Servo_SetSpan(SERVO_HANDLER_T Servo, float32_t span);
void Servo_SetRelSpan(SERVO_HANDLER_T servo, float32_t span);

#endif /* __SERVO_H__       */
