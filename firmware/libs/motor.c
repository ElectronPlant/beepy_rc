/**
 * @file  motor.c
 *
 * @brief Motors are composed by two PWM timer channels and a encoder timer.
 *
 * @ingroup   MotionMotor
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Motor_
 */

#include "plt_assert.h"
#include "plt_defines.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "task.h"

#include "target.h"

#include "encoder.h"
#include "motor.h"
#include "pwm_timer.h"

/** @addtogroup Motion
 *   @{
 */

/** @addtogroup Motor
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define MOTOR_PWM_FREQUENCY_KHZ (10.0f)

/**
 * @brief Critical sections. When adjusting the motor speed it is critical that both channels
 *        are adjusted as close between them as possible. In tasks with FreeRTOS support this will
 *        be achieved with a critical section.
 */
#if PLT_DEFINES_USE_FREE_RTOS == 1
    #include "task.h"
    #define MOTOR_START_CRITICAL_SEC taskENTER_CRITICAL()
    #define MOTOR_EXIT_CRITICAL_SEC  taskEXIT_CRITICAL()
#else
    #define MOTOR_START_CRITICAL_SEC
    #define MOTOR_EXIT_CRITICAL_SEC
#endif

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
  * @brief  Initialize a motor.
  *
  * @param  mot Motor handler.
  *
  * @return DEF_TRUE if successful, DEF_FALSE otherwise.
  */
bool_t Motor_Init(MOTOR_HANDLER_T mot) {
    bool_t ok;
    PLT_ASSERT(NULL != mot);

    ok = Enc_Init(mot->Encoder);
    if (DEF_FALSE == ok) {
        return DEF_FALSE;
    }

    for (uint8_t chn = 0; chn < MOTOR_PWM_CHN_NUM; chn++) {
        ok = PwmTim_InitChn(mot->PwmChn[chn].Timer, mot->PwmChn[chn].Chn, MOTOR_PWM_FREQUENCY_KHZ);
        if (DEF_FALSE == ok) {
            return DEF_FALSE;
        }
    }
    return DEF_TRUE;
}

/**
 * @brief  Start a motor.
 *
 * @param  mot Motor handler.
 */
void Motor_Start(MOTOR_HANDLER_T mot) {
    PLT_ASSERT(NULL != mot);

    Enc_StartEncoder(mot->Encoder);

    for (uint8_t chn = 0; chn < MOTOR_PWM_CHN_NUM; chn++) {
        PwmTim_StartChn(mot->PwmChn[chn].Timer, mot->PwmChn[chn].Chn);
    }
}

/**
 * @brief  Stop a motor.
 *
 * @param  mot Motor handler.
 */
void Motor_Stop(MOTOR_HANDLER_T mot) {
    PLT_ASSERT(NULL != mot);

    Enc_StopEncoder(mot->Encoder);

    for (uint8_t chn = 0; chn < MOTOR_PWM_CHN_NUM; chn++) {
        PwmTim_StopChn(mot->PwmChn->Timer, mot->PwmChn->Chn);
    }
}

/**
 * @brief  Set speed for a motor.
 *
 * @param  mot Motor handler.
 * @param  speed Speed value to set. See notes 1, 2, 3.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. The speed value is saturated to be within MOTOR_MIN_SPEED_VAL and MOTOR_MAX_SPEED_VAL.
 *       2. The sign in the speed value determines the rotation direction. Positive values
 *          will make the motor rotate forwards, while negative values will make the motor drive
 *          backwards.
 *       3. The motor driver operates in PWM mode, see motor_driver.md for further info. The motors
 *          are driven by setting one of the channels low, and the PWM signal on the other channel.
 *          This drivers the motor between the drive and break modes, which is best to control
 *          speed precisely, but may lead to inconsistent rotation at low frequencies.
 *       4. Channel PWM changes should be as close together as possible. Additionally, note that
 *          the change should be first setting the zero duty cycle channel, then, the remaining one.
 */
void Motor_SetSpeed(MOTOR_HANDLER_T mot, float32_t speed) {
    PLT_ASSERT(NULL != mot);
    float32_t speed_sat =
        PLT_UTILS_SATURATE(speed, MOTOR_MIN_SPEED_VAL, MOTOR_MAX_SPEED_VAL); /* See note 1 */

    if (speed_sat <= 0.0f) {
        /* Turn motor forward */
        MOTOR_START_CRITICAL_SEC;
        PwmTim_SetDuty(mot->PwmChn[0].Timer, mot->PwmChn[0].Chn, 0.0f); /* See note 4 */
        PwmTim_SetDuty(mot->PwmChn[1].Timer, mot->PwmChn[1].Chn, -1.0 * speed_sat);
        MOTOR_EXIT_CRITICAL_SEC;
    } else {
        /* Turn motor backwards */
        MOTOR_START_CRITICAL_SEC;
        PwmTim_SetDuty(mot->PwmChn[1].Timer, mot->PwmChn[1].Chn, 0.0f); /* See note 4*/
        PwmTim_SetDuty(mot->PwmChn[0].Timer, mot->PwmChn[0].Chn, speed_sat);
        MOTOR_EXIT_CRITICAL_SEC;
    }
}

/**
 * @brief  Gets the motor's encoder count and direction.
 *
 * @param  mot Motor handler.
 * @param  p_cnt: Pointer to where the encoder count will be stored.
 * @param  p_enc_dir: Pointer to where the encoder count direction will be stored.
 */
void Motor_GetEncoderCnt(MOTOR_HANDLER_T mot, uint32_t* p_cnt, MOTOR_DIRECTION_T* p_enc_dir) {
    PLT_ASSERT(NULL != mot);
    PLT_ASSERT(NULL != p_cnt);
    PLT_ASSERT(NULL != p_enc_dir);

    *p_cnt = Enc_GetCount(mot->Encoder);
    *p_enc_dir = Enc_GetDirection(mot->Encoder);
}


/** @} (end addtogroup Motor)  */
/** @} (end addtogroup Motion)  */
