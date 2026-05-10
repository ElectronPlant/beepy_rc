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
 */

#ifndef __RC_INPUTS_H__
#define __RC_INPUTS_H__

#include "plt_types.h"
#include "std_frame.h"


/********************************************************************************
 * Defines
 ********************************************************************************/
#define RCIN_MAX_AUX_CHN_SUBS (16U)


/********************************************************************************
 * Typedefs
 ********************************************************************************/
/******************************************
 * Control Inputs
 ******************************************/
/**
 * @brief Control inputs are the channels used by the controller to drive the vehicle.
 */
typedef enum RCIN_CONTROL_INPUT_E {
    RCIN_CONTROL_NONE = 0,
    RCIN_CONTROL_THROTTLE,
    RCIN_CONTROL_YAW,
    RCIN_CONTROL_ARM_SWITCH,
    RCIN_CONTROL_MODE,

    RCIN_CONTROL_MAX,
} RCIN_CONTROL_INPUT_T;

typedef struct RCIN_CONTROL_FRAME_S {
    float32_t         Throttle;
    float32_t         Yaw;
    bool_t            ArmSwitch;
    bool_t            Mode; /* TODO define */
    uint32_t          Timestamp;
    STD_FRAME_STATE_T State;
} RCIN_CONTROL_FRAME_T;

typedef struct RCIN_CONTROL_OUTPUT_S {
    RCIN_CONTROL_FRAME_T Frame;
} RCIN_CONTROL_OUTPUT_T;


/******************************************
 * Peripherals
 ******************************************/
/**
 * @brief Peripherals are the different types elements that may be controlled. While instances
 *        are the actual peripherals to be controlled. Any RC channel can be mapped to on or more
 *        entity instances.
 *        Control inputs (e.g. throttle, yaw, etc.) are defined as an different instances of the
 *        control input entity type.
 */
typedef enum RCIN_SERVOS_E {
    RCIN_SERVOS_0 = 0,
    RCIN_SERVOS_1,
    RCIN_SERVOS_2,
    RCIN_SERVOS_3,
    RCIN_SERVOS_4,

    RCIN_SERVOS_MAX,
} RCIN_SERVOS_T;

typedef enum RCIN_LIGHTS_E {
    RCIN_LIGHTS_0 = 0,
    RCIN_LIGHTS_1,
    RCIN_LIGHTS_2,
    RCIN_LIGHTS_3,
    RCIN_LIGHTS_4,

    RCIN_LIGHTS_MAX,
} RCIN_LIGHTS_T;

typedef union RCIN_INSTANCES_U {
    RCIN_CONTROL_INPUT_T ControlIn;
    RCIN_SERVOS_T        Servos;
    RCIN_LIGHTS_T        Lights;
} RCIN_INSTANCES_T;

typedef enum RCIN_TYPE_E {
    RCIN_PER_TYPE_NONE = 0,      /**< Reserved for undefined channels    */
    RCIN_PER_TYPE_CONTROL_INPUT, /**< Control inputs                     */
    RCIN_PER_TYPE_SERVO,         /**< Servo channel                      */
    RCIN_PER_TYPE_LIGHT,         /**< Lights channel                     */

    RCIN_TYPE_MAX,
} RCIN_PER_TYPE_T;

typedef struct RCIN_PERIPHERAL_S {
    RCIN_PER_TYPE_T  Type;
    RCIN_INSTANCES_T Instance;
} RCIN_PERIPHERAL_T;

/******************************************
 * Rate Curves
 ******************************************/
/**
 * @brief Rate curves map the sticks range into a predefined curve to improve the feel.
 *        For example it is best to have more precision in the center of the stick to improve
 *        accuracy. However, if kept constant this will limit the maximum rate at the extreme
 *        end of the stick. A exponential (expo) curve mappings provides the fine grain steps mid
 *        stick and more coarse control at the edges to keep the maximum rates.
 *
 *        Curves also allow adjusting stick drifts by adjusting the middle point of the stick or
 *        a threshold of switch inputs.
 */

typedef enum RCIN_CURVE_TYPES_E {
    RCIN_CURVE_TYPE_NONE = 0, /**< No adjustment, keep as is. */
    RCIN_CURVE_TYPE_EXPO,
    RCIN_CURVE_TYPE_SWITCH,

    RCIN_CURVE_TYPE_MAX,
} RCIN_CURVE_TYPE_T;

typedef union RCIN_CURVE_PARAMS_U {
    struct {
        float32_t Temp; //TODO implement
    } Expo;
    struct {
        float32_t Threshold;
    } Switch;
} RCIN_CURVE_PARAMS_T;

typedef struct RCIN_CURVE_S {
    RCIN_CURVE_TYPE_T   Type;
    RCIN_CURVE_PARAMS_T Params;
} RCIN_CURVE_T;

/******************************************
 * Actions
 ******************************************/
/**
 * @brief Aux RC channels can be mapped used to perform different actions on a peripheral based
 *        on the RC input. The action function is used to define the different actions that
 *        may be taken.
 */
typedef union RCIN_VALUE_U {
    float32_t Analog;
    bool_t    Switch;
} RCIN_VALUE_T;

typedef void (*RCIN_ACTION_FUNC_T)(RCIN_PERIPHERAL_T* p_per, RCIN_VALUE_T val);

/******************************************
 * Subscription
 ******************************************/
/**
 * @brief RC channels can be mapped used to control different peripherals.
 *        The mapping of RC channel to peripheral is done via a subscription pattern.
 *        This enables a single channel to be used for two different things, or some channels
 *        not having anything assigned to them.
 *
 *        The subscriptions for the control inputs are simpler since they only need to map the
 *        control input to the current control variable.
 *
 *        The subscriptions for the aux channels need to specify the peripheral and the action
 *        that needs to be executed.
 */

typedef struct RCIN_CHN_CTRL_SUBS_S {
    uint8_t              Chn; /**< Channel the subscription is linked to. */
    RCIN_CONTROL_INPUT_T Input;
    RCIN_CURVE_T         Curve; /**< Curve to adjust the channel ouput. */
} RCIN_CHN_CTRL_SUBS_T;

typedef struct RCIN_CHN_AUX_SUBS_S {
    uint8_t            Chn;         /**< Channel the subscription is linked to.         */
    RCIN_PERIPHERAL_T  Per;         /**< Peripheral subscribed to the channel.          */
    RCIN_CURVE_T       Curve;       /**< Curve to adjust the channel ouput.             */
    RCIN_ACTION_FUNC_T ActFunc_Ptr; /**< Pointer to the function to execute the action  */
} RCIN_CHN_AUX_SUBS_T;
//TODO for now actions will be inferred from the type, but there is a change that a peripheral
//     may have more than one action, so they need to be linked somehow. The approach will be
//     configuring the action on the subscription (or the aux subscriptions only).

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
void RcIn_SetConfig(RCIN_CHN_CTRL_SUBS_T* p_chn_subs, RCIN_CHN_AUX_SUBS_T* p_aux_subs);
void RcIn_HandleRcFrame(STD_FRAME_T* p_rc_frame); // TODO rename function


#endif /* __RC_INPUTS_H__   */
