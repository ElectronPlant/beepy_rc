/**
 * @file  servo.c
 *
 * @brief Driver for analog servo motors.
 *        Analog servos allow setting their angle based on the duty cycle of their PWM control
 *        signal. The signal has a period of 20ms (50Hz) and the duration of 1ms (for 0deg) and
 *        a duration of 2ms (for 180deg).
 *
 * @ingroup   MotionServo
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Servo_
 */

#include "plt_assert.h"
#include "plt_defines.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "task.h"

#include "target.h"

#include "pwm_timer.h"
#include "servo.h"

/** @addtogroup Motion
 *   @{
 */

/** @addtogroup Servo
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define SERVO_PWM_PERIOD_MS     (20.0f)
#define SERVO_PWM_FREQUENCY_KHZ (1 / SERVO_PWM_PERIOD_MS)

/** @note This is for the MG90s servos, which are mostly generic. Other servos will require other
 *        configs.
 *        1) There are servos that are designed with only 90deg rotation.
 *        2) MG90S servos read a signal between 0.5 - 2.5ms but 1-2ms range is also standard.
 */
#define SERVO_MAX_ANGLE                      (180.0f)
#define SERVO_MIN_ANGLE                      (0.0f)
#define SERVO_MAX_ANGLE_DURATION_MS          (2.5f)
#define SERVO_MIN_ANGLE_DURATION_MS          (0.5f)
#define SERVO_DURATION_TO_DUTY_PERCENT(X_MS) (100.0f * X_MS / SERVO_PWM_PERIOD_MS)


/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static float32_t Servo_DutyFromAngle(float32_t angle);
static float32_t Servo_DutyFromPercentage(float32_t pct);


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
  * @brief  Initialize a servo motor.
  *
  * @param  servo Servo handler.
  *
  * @return DEF_TRUE if successful, DEF_FALSE otherwise.
  */
bool_t Servo_Init(SERVO_HANDLER_T servo) {
    bool_t ok;
    PLT_ASSERT(NULL != servo);

    ok = PwmTim_InitChn(servo->Timer, servo->Chn, SERVO_PWM_FREQUENCY_KHZ);
    if (DEF_FALSE == ok) {
        return DEF_FALSE;
    }
    return DEF_TRUE;
}

/**
 * @brief  Start a servo motor.
 *
 * @param  servo Servo handler.
 */
void Servo_Start(SERVO_HANDLER_T servo) {
    PLT_ASSERT(NULL != servo);

    PwmTim_StartChn(servo->Timer, servo->Chn);
}

/**
 * @brief  Stop a servo motor.
 *
 * @param  servo Servo handler.
 */
void Servo_Stop(SERVO_HANDLER_T servo) {
    PLT_ASSERT(NULL != servo);

    PwmTim_StopChn(servo->Timer, servo->Chn);
}

/**
 * @brief  Calculate the required duty cycle to set the specified angle.
 *
 * @param  angle Target angle to set.
 *
 * @return duty cycle to set.
 */
static float32_t Servo_DutyFromAngle(float32_t angle) {
    float32_t angle_dur = PltUtils_MapF32(
        angle,
        SERVO_MIN_ANGLE,
        SERVO_MAX_ANGLE,
        SERVO_MIN_ANGLE_DURATION_MS,
        SERVO_MAX_ANGLE_DURATION_MS
    );
    return SERVO_DURATION_TO_DUTY_PERCENT(angle_dur);
}

/**
 * @brief  Calculate the required duty cycle for the specified span percentage.
 *
 * @param  pct Span percentage to set.
 *
 * @return duty cycle to set.
 */
static float32_t Servo_DutyFromPercentage(float32_t pct) {
    float32_t pct_dur = PltUtils_MapF32(
        pct,
        SERVO_MIN_PERCENTAGE,
        SERVO_MAX_PERCENTAGE,
        SERVO_MIN_ANGLE_DURATION_MS,
        SERVO_MAX_ANGLE_DURATION_MS
    );
    return SERVO_DURATION_TO_DUTY_PERCENT(pct_dur);
}

/**
 * @brief  Set angle for a servo motor.
 *
 * @param  servo Servo handler.
 * @param  angle Set angle. See note 1.
 *
 * @note List of notes:
 *       1. The angle must be between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE. The angle will be
 *          saturated between the limit values.
 */
void Servo_SetAngle(SERVO_HANDLER_T servo, float32_t angle) {
    PLT_ASSERT(NULL != servo);
    float32_t sat_angle = PLT_UTILS_SATURATE(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

    float32_t duty = Servo_DutyFromAngle(sat_angle);
    PwmTim_SetDuty(servo->Timer, servo->Chn, duty);
}

/**
 * @brief  Set the servo angle as a percentage of the total servo span.
 *         There are some servo motors that rotate 180 degrees, while others only rotate 90
 *         degrees. Thus, to directly set the servo angle, the model needs to be known a priory.
 *         In most cases it may be more usefull the control the servo angle based on the percentage
 *         of the total rotation span.
 *
 * @param  servo  Servo handler.
 * @param  travel Set point for the servo angle. See note 1
 *
 * @note List of notes:
 *       1. Any span value outside the range [SERVO_MIN_PERCENTAGE, SERVO_MAX_PERCENTAGE] will be
 *          saturated between the limit values.
 */
void Servo_Set(SERVO_HANDLER_T servo, float32_t travel) {
    PLT_ASSERT(NULL != servo);
    float32_t sat_travel = PLT_UTILS_SATURATE(travel, SERVO_MIN_PERCENTAGE, SERVO_MAX_PERCENTAGE);

    float32_t duty = Servo_DutyFromPercentage(sat_travel);
    PwmTim_SetDuty(servo->Timer, servo->Chn, duty);
}


/** @} (end addtogroup Servo)  */
/** @} (end addtogroup Motion)  */
