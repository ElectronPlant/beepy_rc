/**
 * @file  rc_subs.h
 * @brief Translator from RC frame to control input.
 *        Handles translating RC channels to an specific control parameter.
 *        This mapping is done via a subscription pattern. Thus, a single channel may be used for
 *        two different control inputs, or not be mapped to anything.
 *        Additionally, RC inputs can be adjusted using specific curves to fine tune the controlled
 *        characteristics (e.g. adjusting the gain, offset, adding a deadband, etc.).
 *
 *        Control inputs are classified in two types:
 *          @li Drive inputs: which adjust the vehicle's movement (e.g. throttle, yaw,
 *              arm switch, etc.). These are common for all boards and vehicles.
 *          @li Aux inputs, which controls the remaining peripherals (e.g. servos, lights, buzzer,
 *              etc.). These are specific to the board being used.
 *
 *        Both control input types are handled using different subscription structs.
 *
 * @ingroup   RcInputs
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __RC_SUBS_H__
#define __RC_SUBS_H__

#include "plt_types.h"

#include "curves.h"
#include "peripherals.h"

/********************************************************************************
 * Defines
 ********************************************************************************/
#define RCSUBS_INVALID_RC_CHANNEL (255) /* Sentinel channel value to indicate no channel. */


/********************************************************************************
 * Typedefs
 ********************************************************************************/
/**
 * @brief Drive inputs are used for the vehicle navigation.
 */
typedef enum RCSUBS_DRIVE_SETPOINT_E {
    RCSUBS_DRIVE_SETPOINT_THROTTLE,
    RCSUBS_DRIVE_SETPOINT_YAW,
    RCSUBS_DRIVE_SETPOINT_ARM_SWITCH,

    /* Unsupported for ground vehicles. */
    /*
    RCSUBS_DRIVE_SETPOINT_ROLL,
    RCSUBS_DRIVE_SETPOINT_PITCH,
    */

    RCSUBS_DRIVE_SETPOINT_MAX,
} RCSUBS_DRIVE_SETPOINT_T;

/******************************************
 * Subscription
 ******************************************/
/**
 * @brief Drive input subscription.
 *        Mapping needs to be done following the order specified by RCSUBS_DRIVE_SETPOINT_T.
 */
typedef struct RCSUBS_DRIVE_INPUTS_S {
    uint8_t                 Chn; /**< Channel the subscription is linked to. */
    RCSUBS_DRIVE_SETPOINT_T Input;
    CURVES_T                Curve; /**< Curve to adjust the channel ouput. */
} RCSUBS_DRIVE_INPUTS_T;

/**
 * @brief Aux input subscription.
 *        Mapping can be done in any order. If not all aux channels are used, the last subscription
 *        must be done to the RCSUBS_INVALID_RC_CHANNEL, which indicates the end of the aux
 *        subscriptions.
 */
typedef struct RCSUBS_AUX_INPUTS_S {
    uint8_t          Chn;   /**< Channel the subscription is linked to. */
    PER_PERIPHERAL_T Per;   /**< Peripheral subscribed to the channel.  */
    CURVES_T         Curve; /**< Curve to adjust the channel ouput.     */
} RCSUBS_AUX_INPUTS_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

#endif /* __RC_SUBS_H__   */
