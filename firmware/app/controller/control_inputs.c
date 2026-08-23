/**
 * @file  control_inputs.h
 * @brief Process RC subscriptions.
 *
 * @ingroup   ControlInputs
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: CIn_
 */

#include <string.h>

#include "plt_assert.h"
#include "plt_types.h"

#include "target.h"

#include "control_inputs.h"
#include "controller.h"
#include "curves.h"
#include "model.h"
#include "peripherals.h"
#include "std_frame.h"

/** @addtogroup Controller
 *    @{
 */

/** @addtogroup ControlInputs
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

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
/******************************************
 * Manage subscriptions
 ******************************************/

/**
 * @brief  Initialize subscriptions.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *      1. Drive subscriptions do not need to be initialized.
 */
bool_t CIn_Init(void) {
    bool_t ok = DEF_TRUE;
    bool_t done = DEF_FALSE;

    /* Initialize aux subscriptions, see note 1. */
    for (uint8_t i = 0; MODEL_MAX_RC_SETPOINT_AUX_INPUTS > i && DEF_FALSE == done && DEF_TRUE == ok;
         i++) {

        if (PER_TYPE_NONE == Target_AuxSubs[i].Per.Type) {
            done = DEF_TRUE;
        } else {
            ok = Per_Init(&Target_AuxSubs[i].Per);
        }
    }
    return ok;
}

/**
 * @brief  Initialize aux subscriptions.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *      1. Drive subscriptions do not need to be initialized.
 */
void CIn_Start(void) {
    bool_t done = DEF_FALSE;

    /* Initialize aux subscriptions, see note 1. */
    for (uint8_t i = 0; MODEL_MAX_RC_SETPOINT_AUX_INPUTS > i && DEF_FALSE == done; i++) {

        if (PER_TYPE_NONE == Target_AuxSubs[i].Per.Type) {
            done = DEF_TRUE;
        } else {
            Per_Start(&Target_AuxSubs[i].Per, Target_AuxSubs[i].InitialValue);
        }
    }
}

/**
 * @brief  Run aux subscriptions.
 */
void CIn_RunAux(MODEL_RC_SETPOINT_T* p_rc_setpoint) {
    bool_t done = DEF_FALSE;
    for (uint8_t i = 0; MODEL_MAX_RC_SETPOINT_AUX_INPUTS > i && DEF_FALSE == done; i++) {

        if (PER_TYPE_NONE == Target_AuxSubs[i].Per.Type) {
            done = DEF_TRUE;
        } else {
            Per_ApplySetpoint(&Target_AuxSubs[i].Per, p_rc_setpoint->AuxInputValues[i]);
        }
    }
}

/******************************************
 * Handle RC Frame
 ******************************************/
/**
 * @brief  Translate the RC state.
 *
 * @param p_rc_frame Pointer to the RC frame to process.
 * @param p_output   Pointer to the output RC setpoint.
 */
static void CIn_TranslateState(STD_FRAME_T* p_rc_frame, MODEL_RC_SETPOINT_T* p_output) {
    MODEL_RC_SETPOINT_STATE_T state = MODEL_RC_SETPOINT_STATE_PENDING;
    switch (p_rc_frame->State) {
        case STD_FRAME_STATE_VALID:
            state = MODEL_RC_SETPOINT_STATE_VALID;
            break;
        default:
            state = MODEL_RC_SETPOINT_STATE_DISCONNECTED;
            break;
    }
    p_output->State = state;
}

/**
 * @brief  Processes the control inputs of the RC frame.
 *
 * @param p_rc_frame Pointer to the RC frame to process.
 * @param p_output   Pointer to the output RC setpoint.
 */
static void CIn_ProcessControlInputs(STD_FRAME_T* p_rc_frame, MODEL_RC_SETPOINT_T* p_output) {
    for (uint8_t i = 0; MODEL_MAX_RC_SETPOINT_DRIVE_INPUTS > i; i++) {
        float32_t temp;
        Curves_ApplyCurve(
            &Target_DriveSubs[i].Curve,
            p_rc_frame->Channels[Target_DriveSubs[i].Chn],
            &temp
        );
        p_output->DriveInputs[Target_DriveSubs[i].Input] = temp;
    }
}

/**
 * @brief  Processes the aux inputs of the RC frame.
 *
 * @param p_rc_frame Pointer to the RC frame to process.
 * @param p_output   Pointer to the output RC setpoint.
 */
static void CIn_ProcessAuxInputs(STD_FRAME_T* p_rc_frame, MODEL_RC_SETPOINT_T* p_output) {
    bool_t done = DEF_FALSE;
    for (uint8_t i = 0; MODEL_MAX_RC_SETPOINT_AUX_INPUTS > i && DEF_FALSE == done; i++) {
        if (PER_TYPE_NONE == Target_AuxSubs[i].Per.Type) {
            done = DEF_TRUE;
        } else {
            float32_t temp;
            Curves_ApplyCurve(
                &Target_AuxSubs[i].Curve,
                p_rc_frame->Channels[Target_AuxSubs[i].Chn],
                &temp
            );
            p_output->AuxInputValues[i] = temp;
        }
    }
}

/**
 * @brief  Handles a new RC frame.
 *         Processes the inputs notifies the control channels, and manages the aux inputs.
 *
 * @param p_rc_frame Pointer to the RC frame to manage.
 */
void CIn_HandleRcFrame(STD_FRAME_T* p_rc_frame) {
    MODEL_RC_SETPOINT_T output = {
        .Timestamp = p_rc_frame->RxTime,
    };

    /* State */
    CIn_TranslateState(p_rc_frame, &output);
    /* Drive Inputs */
    CIn_ProcessControlInputs(p_rc_frame, &output);
    /* Aux Inputs */
    CIn_ProcessAuxInputs(p_rc_frame, &output);

    /* Update the RC setpoint, see note 1 */
    Model_UpdateRcSetpoint(&output);
}


/** @} (end addtogroup ControlInputs)  */
/** @} (end addtogroup Controller)  */
