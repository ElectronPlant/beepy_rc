/**
 * @file  rc_inputs.h
 * @brief Definition of the RC input channel types and config.
 *
 * @ingroup   RcInputs
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: RcIn_
 */

#include "plt_assert.h"
#include "plt_types.h"

#include "controller.h"
#include "model.h"
#include "rc_inputs.h"
#include "std_frame.h"


/** @addtogroup RcInputs
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define RCIN_CONTROL_INPUTS_MAX (RCIN_CONTROL_MAX - 1U)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void RcIn_ProcessControlInputs(STD_FRAME_T* p_rc_frame);
static void RcIn_ProcessAuxInputs(STD_FRAME_T* p_rc_frame);

static MODEL_RC_SETPOINT_STATE_T RcIn_TranslateState(STD_FRAME_T* p_rc_frame);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
static RCIN_CHN_CTRL_SUBS_T RcIn_ControlSubs[RCIN_CONTROL_INPUTS_MAX] = {
    {.Chn = 0U, .Input = RCIN_CONTROL_THROTTLE, .Curve = {.Type = RCIN_CURVE_TYPE_NONE}},
    {.Chn = 1U, .Input = RCIN_CONTROL_YAW, .Curve = {.Type = RCIN_CURVE_TYPE_NONE}},
    {.Chn = 10U, .Input = RCIN_CONTROL_ARM_SWITCH, .Curve = {.Type = RCIN_CURVE_TYPE_NONE}},
    {.Chn = 0U, .Input = RCIN_CONTROL_NONE, .Curve = {.Type = RCIN_CURVE_TYPE_NONE}},
};

// TODO define aux hardcoded values.
static RCIN_CHN_AUX_SUBS_T RcIn_AuxSubs[RCIN_MAX_AUX_CHN_SUBS] = {
    {.Chn = 0U,
     .Per = {.Type = RCIN_PER_TYPE_NONE},
     .Curve = {.Type = RCIN_CURVE_TYPE_NONE},
     .ActFunc_Ptr = NULL},
};


/********************************************************************************
 * Function Implementations
 ********************************************************************************/
/**
 * @brief  Sets a new RC input config.
 *
 * @param  p_ctrlr_subs Pointer to the control channel subscriptions.
 * @param  p_aux_susbs  Pointer to the aux channel subscriptions.
 */
void RcIn_SetConfig(RCIN_CHN_CTRL_SUBS_T* p_ctrl_subs, RCIN_CHN_AUX_SUBS_T* p_aux_subs) {
    (void)p_ctrl_subs;
    (void)p_aux_subs;
    (void)RcIn_AuxSubs;
    PLT_UNIMPLEMENTED; // TODO pending to be implemented
}

/**
 * @brief  Handles a new RC frame.
 *         Processes the inputs notifies the control channels, and manages the aux inputs.
 *
 * @param p_rc_frame Pointer to the RC frame to manage.
 */
void RcIn_HandleRcFrame(STD_FRAME_T* p_rc_frame) {
    RcIn_ProcessControlInputs(p_rc_frame);
    RcIn_ProcessAuxInputs(p_rc_frame);
}

static MODEL_RC_SETPOINT_STATE_T RcIn_TranslateState(STD_FRAME_T* p_rc_frame) {
    MODEL_RC_SETPOINT_STATE_T state = MODEL_RC_SETPOINT_STATE_PENDING;
    switch (p_rc_frame->State) {
        case STD_FRAME_STATE_VALID:
            state = MODEL_RC_SETPOINT_STATE_VALID;
            break;
        default:
            state = MODEL_RC_SETPOINT_STATE_FAILSAFE;
            break;
    }
    return state;
}

/**
 * @brief  Processes the control inputs of the RC frame.
 *
 * @param p_rc_frame Pointer to the RC frame to manage.
 */
static void RcIn_ProcessControlInputs(STD_FRAME_T* p_rc_frame) {
    MODEL_RC_SETPOINT_T output = {.State = RcIn_TranslateState(p_rc_frame)};

    for (uint8_t i = 0; RCIN_CONTROL_INPUTS_MAX > i; i++) {
        switch (RcIn_ControlSubs[i].Input) {
            case RCIN_CONTROL_THROTTLE:
                output.Throttle =
                    (float32_t)p_rc_frame->Channels[RcIn_ControlSubs[i].Chn]; // TODO add curves
                break;
            case RCIN_CONTROL_YAW:
                output.Yaw = (float32_t)p_rc_frame->Channels[RcIn_ControlSubs[i].Chn];
                break;
            case RCIN_CONTROL_ARM_SWITCH:
                output.ArmSwitch =
                    (bool_t)(p_rc_frame->Channels[RcIn_ControlSubs[i].Chn] < 0 ? DEF_FALSE
                                                                               : DEF_TRUE);
                break;
            case RCIN_CONTROL_NONE:
                /* - No-op - */
                break;
            default:
                PLT_UNREACHABLE;
        }
    }

    /* Update the RC setpoint, see note 1 */
    Model_UpdateRcSetpoint(&output);
}

/**
 * @brief  Processes the aux inputs of the RC frame.
 *
 * @param p_rc_frame Pointer to the RC frame to manage.
 */
static void RcIn_ProcessAuxInputs(STD_FRAME_T* p_rc_frame) {
    (void)p_rc_frame;
    // TODO implement this function.
}


/** @} (end addtogroup RcInputs)  */
