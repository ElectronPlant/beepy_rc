/**
 * @file  fwd.c
 * @brief Controller for the fwd (four wheel drive) vehicle.
 *
 * @ingroup   Fwd
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Fwd_
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "target.h"

#include "fwd.h"
#include "motor.h"
#include "rc_subs.h"


/** @addtogroup App
 *    @{
 */

/** @addtogroup Controller
 *    @{
 */

/** @addtogroup Vehicles
 *    @{
 */

/** @addtogroup Fwd
 *    @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
#define FWD_MOTOR_DISARM_DUTY (0.0f)


/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum FWD_MOTOR_NAME_E {
    FRONT_LEFT = 0,
    FRONT_RIGHT,
    BACK_LEFT,
    BACK_RIGHT,

    FWD_REQUIRED_MOTORS,
} FWD_MOTOR_NAME_T;

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/
/**
 * @brief The translator contains the target indexes ordered as defined by FWD_MOTOR_NAME_T.
 */
static uint8_t Fwd_Config2TargetTranslator[FWD_REQUIRED_MOTORS] = {0, 1, 2, 3};

static const float32_t Fwd_MotorGains[] = {1.0f, -1.0f, 1.0f, -1.0f};
/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  FWD implementation of the init interface.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Fwd_Init(void) {
    PLT_BUILD_ASSERT(FWD_REQUIRED_MOTORS <= TARGET_MOTOR_NUM);

    bool_t ok = DEF_TRUE;

    for (uint8_t m = 0; m < FWD_REQUIRED_MOTORS && DEF_TRUE == ok; m++) {
        uint8_t target_index = Fwd_Config2TargetTranslator[m];
        ok = Motor_Init((MOTOR_HANDLER_T)&Target_Motors[target_index]);
    }

    return ok;
}

/**
 * @brief  FWD implementation of the start interface.
 */
void Fwd_Start(void) {
    for (uint8_t m = 0; m < FWD_REQUIRED_MOTORS; m++) {
        uint8_t target_index = Fwd_Config2TargetTranslator[m];
        Motor_Start((MOTOR_HANDLER_T)&Target_Motors[target_index]);
    }
}

/**
 * @brief  FWD implementation of the stop interface.
 */
void Fwd_Stop(void) {
    for (uint8_t m = 0; m < FWD_REQUIRED_MOTORS; m++) {
        uint8_t target_index = Fwd_Config2TargetTranslator[m];
        Motor_Stop((MOTOR_HANDLER_T)&Target_Motors[target_index]);
    }
}

/**
 * @brief  FWD implementation of the RunControlLoop Interface.
 */
void Fwd_RunControlLoop(MODEL_RC_SETPOINT_T* p_frame) {
    float32_t throttle = p_frame->DriveInputs[RCSUBS_DRIVE_SETPOINT_THROTTLE];
    float32_t vang = p_frame->DriveInputs[RCSUBS_DRIVE_SETPOINT_YAW];
    float32_t throttle_abs = PLT_UTILS_ABS(throttle);
    float32_t vang_abs = PLT_UTILS_ABS(vang);

    if (throttle_abs + vang_abs > MOTOR_MAX_SPEED_VAL) {
        float32_t delta = MOTOR_MAX_SPEED_VAL - vang_abs - throttle_abs;
        if (throttle < 0.0f) {
            throttle += delta;
        } else {
            throttle -= delta;
        }
    }

    float32_t t_r = (throttle + vang);
    float32_t t_l = (throttle - vang);

    /* Update motors */
    Motor_SetSpeed(
        (MOTOR_HANDLER_T)&Target_Motors[Fwd_Config2TargetTranslator[FRONT_LEFT]],
        t_l * Fwd_MotorGains[FRONT_LEFT]
    );
    Motor_SetSpeed(
        (MOTOR_HANDLER_T)&Target_Motors[Fwd_Config2TargetTranslator[BACK_LEFT]],
        t_l * Fwd_MotorGains[BACK_LEFT]
    );
    Motor_SetSpeed(
        (MOTOR_HANDLER_T)&Target_Motors[Fwd_Config2TargetTranslator[FRONT_RIGHT]],
        t_r * Fwd_MotorGains[FRONT_RIGHT]
    );
    Motor_SetSpeed(
        (MOTOR_HANDLER_T)&Target_Motors[Fwd_Config2TargetTranslator[BACK_RIGHT]],
        t_r * Fwd_MotorGains[BACK_RIGHT]
    );
}

/**
 * @brief  FWD implementation of the Disarm interface.
 */
void Fwd_Disarm(void) {
    /* Update motors */
    Motor_SetSpeed(
        (MOTOR_HANDLER_T)&Target_Motors[Fwd_Config2TargetTranslator[FRONT_LEFT]],
        FWD_MOTOR_DISARM_DUTY
    );
    Motor_SetSpeed(
        (MOTOR_HANDLER_T)&Target_Motors[Fwd_Config2TargetTranslator[BACK_LEFT]],
        FWD_MOTOR_DISARM_DUTY
    );
    Motor_SetSpeed(
        (MOTOR_HANDLER_T)&Target_Motors[Fwd_Config2TargetTranslator[FRONT_RIGHT]],
        FWD_MOTOR_DISARM_DUTY
    );
    Motor_SetSpeed(
        (MOTOR_HANDLER_T)&Target_Motors[Fwd_Config2TargetTranslator[BACK_RIGHT]],
        FWD_MOTOR_DISARM_DUTY
    );
}

/** @} (end addtogroup Fwd)         */
/** @} (end addtogroup Vehicles)    */
/** @} (end addtogroup Controller)  */
/** @} (end addtogroup App)         */
