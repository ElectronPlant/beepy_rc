/**
 * @file  model.h
 * @brief Maintainer for the stick inputs and vehicle attitude model.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __MODEL_H__
#define __MODEL_H__

#include "plt_types.h"

#include "peripherals.h"
#include "rc_subs.h"
#include "target.h"


/********************************************************************************
 * Defines
 ********************************************************************************/
#define MODEL_MAX_RC_SETPOINT_DRIVE_INPUTS (RCSUBS_DRIVE_SETPOINT_MAX)
#define MODEL_MAX_RC_SETPOINT_AUX_INPUTS   (TARGET_NUM_AUX_PERIPHERALS)


/********************************************************************************
 * Typedefs
 ********************************************************************************/

/******************************************
 * Position & Attitude
 ******************************************/
typedef struct MODEL_POS_S {
    float32_t Disp;  /**< Polar coordinates, radius from the center. */
    float32_t Theta; /**< Polar coordinates, angle  */
} MODEL_POS_T;

/**
 * @note For the moment only the attitude is needed for the vehicle since it is assumed to be
 *       always flat on the ground. Thus, pitch and roll are assumed to be zero.
 */
typedef struct MODEL_ATTITUDE_S {
    float32_t Yaw; /**< Orientation of the vehicle */
} MODEL_ATTITUDE_T;


/******************************************
 * RC Setpoint
 ******************************************/
typedef enum MODEL_RC_SETPOINT_STATE_E {
    MODEL_RC_SETPOINT_STATE_PENDING = 0,
    MODEL_RC_SETPOINT_STATE_VALID,
    MODEL_RC_SETPOINT_STATE_DISCONNECTED,
} MODEL_RC_SETPOINT_STATE_T;

typedef struct MODEL_RC_SETPOINT_S {
    float32_t                 DriveInputs[MODEL_MAX_RC_SETPOINT_DRIVE_INPUTS];
    PER_PERIPHERAL_T          AuxInputs[MODEL_MAX_RC_SETPOINT_AUX_INPUTS];
    float32_t                 AuxInputValues[MODEL_MAX_RC_SETPOINT_AUX_INPUTS];
    uint32_t                  Timestamp;
    MODEL_RC_SETPOINT_STATE_T State;
} MODEL_RC_SETPOINT_T;

typedef enum MODEL_NOTIFY_SOURCE_E {
    MODEL_NOTIFY_SOURCE_RC = 0,
    MODEL_NOTIFY_SOURCE_POS,
    MODEL_NOTIFY_SOURCE_ATTITUDE,

    MODEL_NOTIFY_SOURCE_MAX,
} MODEL_NOTIFY_SOURCE_T;

typedef void (*MODEL_UPDATE_NOTICE_FUNC)(MODEL_NOTIFY_SOURCE_T source);


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t Model_Init(MODEL_UPDATE_NOTICE_FUNC p_update_funct);
void   Model_UpdateRcSetpoint(MODEL_RC_SETPOINT_T* p_frame);
void   Model_UpdatePosition(MODEL_POS_T* p_pos);
void   Model_UpdateAttitude(MODEL_ATTITUDE_T* p_attitude);

void Model_GetRcSetpoint(MODEL_RC_SETPOINT_T* p_frame);
void Model_GetPosition(MODEL_POS_T* p_pos);
void Model_GetAttitude(MODEL_ATTITUDE_T* p_attitude);

#endif /* __MODELS_H__      */
