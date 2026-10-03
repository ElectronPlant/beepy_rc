/**
 * @file  model.h
 * @brief Maintainer for the stick inputs and vehicle attitude model.
 *        This library may be used by different tasks since it is mutex protected.
 *
 * @ingroup   Model
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#include <string.h>
#include "plt_assert.h"
#include "plt_types.h"

#include "FreeRTOS.h"
#include "task.h"

#include "model.h"
#include "peripherals.h"


/** @addtogroup Model
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef struct MODEL_S {
    MODEL_RC_SETPOINT_T      RcSetpoint;
    MODEL_POS_T              Pos;
    MODEL_ATTITUDE_T         Attitude;
    MODEL_UPDATE_NOTICE_FUNC UpdateFunct_Ptr;
} MODEL_T;


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/
static MODEL_T Model_Model = {
    .RcSetpoint =
        {
            .State = MODEL_RC_SETPOINT_STATE_PENDING,
            .Timestamp = 0,
            .DriveInputs = {0},
            .AuxInputValues = {0},
        },
    .Attitude = {.Yaw = 0.0f},
    .Pos =
        {
            .Disp = 0.0f,
            .Theta = 0.0f,
        },
    .UpdateFunct_Ptr = NULL,
};


/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Initialize the model library.
 *
 * @param update_funct Pointer to the function to notify that the model has been updated.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Model_Init(MODEL_UPDATE_NOTICE_FUNC update_funct) {
    bool_t ok;
    /* Force it to be a singleton */
    ok = NULL == Model_Model.UpdateFunct_Ptr && NULL != update_funct ? DEF_TRUE : DEF_FALSE;
    if (DEF_TRUE == ok) {
        Model_Model.UpdateFunct_Ptr = update_funct;
    }
    return ok;
}

/******************************************
 * Update functions
 ******************************************/
/**
 * @brief  Updates the RC setpoint defined in the model.
 *         This function is ment to be called by other tasks, and updates the model using
 *         critical sections.
 *
 * @param  p_frame Pointer to the RC setpoint to copy into the model.
 */
void Model_UpdateRcSetpoint(MODEL_RC_SETPOINT_T* p_frame) {
    PLT_ASSERT(NULL != p_frame);
    PLT_ASSERT(NULL != Model_Model.UpdateFunct_Ptr);

    taskENTER_CRITICAL();
    memcpy(&Model_Model.RcSetpoint, p_frame, sizeof(MODEL_RC_SETPOINT_T));
    taskEXIT_CRITICAL();

    Model_Model.UpdateFunct_Ptr(MODEL_NOTIFY_SOURCE_RC);
}

/**
 * @brief  Updates the Position estimate defined in the model.
 *         This function is ment to be called by other tasks, and updates the model using
 *         critical sections.
 *
 * @param  p_pos Pointer to the new position estimate.
 */
void Model_UpdatePosition(MODEL_POS_T* p_pos) {
    PLT_ASSERT(NULL != p_pos);
    PLT_ASSERT(NULL != Model_Model.UpdateFunct_Ptr);

    taskENTER_CRITICAL();
    memcpy(&Model_Model.Pos, p_pos, sizeof(MODEL_POS_T));
    taskEXIT_CRITICAL();

    Model_Model.UpdateFunct_Ptr(MODEL_NOTIFY_SOURCE_POS);
}

/**
 * @brief  Updates the Position estimate defined in the model.
 *         This function is ment to be called by other tasks, and updates the model using
 *         critical sections.
 *
 * @param  p_attitude Pointer to the new attitude estimate.
 */
void Model_UpdateAttitude(MODEL_ATTITUDE_T* p_attitude) {
    PLT_ASSERT(NULL != p_attitude);
    PLT_ASSERT(NULL != Model_Model.UpdateFunct_Ptr);

    taskENTER_CRITICAL();
    memcpy(&Model_Model.Attitude, p_attitude, sizeof(MODEL_ATTITUDE_T));
    taskEXIT_CRITICAL();

    Model_Model.UpdateFunct_Ptr(MODEL_NOTIFY_SOURCE_ATTITUDE);
}

/******************************************
 * Getter functions
 ******************************************/
/**
 * @brief  Getter function for the RC setpoint data.
 *         This function creates a copy of the model using critical sections.
 *
 * @param  p_frame Pointer where the RC setpoint will be stored.
 */
void Model_GetRcSetpoint(MODEL_RC_SETPOINT_T* p_frame) {
    taskENTER_CRITICAL();
    memcpy(p_frame, &Model_Model.RcSetpoint, sizeof(MODEL_RC_SETPOINT_T));
    taskEXIT_CRITICAL();
}

/**
 * @brief  Getter function for the position estimate data.
 *         This function creates a copy of the model using critical sections.
 *
 * @param  p_pos Pointer where the position estimate will be stored.
 */
void Model_GetPosition(MODEL_POS_T* p_pos) {
    taskENTER_CRITICAL();
    memcpy(p_pos, &Model_Model.Pos, sizeof(MODEL_POS_T));
    taskEXIT_CRITICAL();
}

/**
 * @brief  Getter function for the attitude estimate data.
 *         This function creates a copy of the model using critical sections.
 *
 * @param  p_attitude Pointer where the position estimate will be stored.
 */
void Model_GetAttitude(MODEL_ATTITUDE_T* p_attitude) {
    taskENTER_CRITICAL();
    memcpy(p_attitude, &Model_Model.Attitude, sizeof(MODEL_POS_T));
    taskEXIT_CRITICAL();
}


/** @} (end addtogroup Model)  */
