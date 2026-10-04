/**
 * @file  motor.h
 * @brief Motor driver.
 *        Motor are composed by two PWM channels and encoder timer.
 *
 * @ingroup   Motor
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "plt_types.h"

#include "encoder.h"
#include "pwm_timer.h"


/** @addtogroup Libs
 *   @{
 */

/** @addtogroup Motor
 *    @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
#define MOTOR_PWM_CHN_NUM (2U)

#define MOTOR_MAX_SPEED_VAL (100.0f)
#define MOTOR_MIN_SPEED_VAL (-100.0f)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

typedef struct MOTOR_PWM_CHN_S {
    PWM_TIM_HANDLER_T  Timer;
    PWM_TIM_CHANNELS_T Chn;
} MOTOR_PWM_T;

typedef struct MOTOR_S {
    MOTOR_PWM_T   PwmChn[MOTOR_PWM_CHN_NUM];
    ENC_HANDLER_T Encoder;
} MOTOR_T;

typedef MOTOR_T* MOTOR_HANDLER_T;

/* Direction */
typedef ENC_DIRECTION_T MOTOR_DIRECTION_T;

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t Motor_Init(MOTOR_HANDLER_T mot);
void   Motor_Start(MOTOR_HANDLER_T mot);
void   Motor_Stop(MOTOR_HANDLER_T mot);

void Motor_SetSpeed(MOTOR_HANDLER_T mot, float32_t speed);
void Motor_GetEncoderCnt(MOTOR_HANDLER_T mot, uint32_t* p_cnt, MOTOR_DIRECTION_T* p_enc_dir);


/** @} (end addtogroup Motor)   */
/** @} (end addtogroup Libs)    */

#endif /* __MOTOR_H__       */
