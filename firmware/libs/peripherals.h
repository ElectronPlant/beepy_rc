/**
 * @file  peripherals.h
 * @brief Generic driver for the controllable peripherals.
 *        Controllable peripherals are all components that may be remotely controlled, which are
 *        not part of the main drive module (e.g. servos, lights, buzzer, etc.).
 *
 * @ingroup   Peripherals
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PERIPHERALS_H__
#define __PERIPHERALS_H__

#include "plt_types.h"


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum PER_SERVOS_E {
    PER_SERVOS_0 = 0,
    PER_SERVOS_1,
    PER_SERVOS_2,
    PER_SERVOS_3,
    PER_SERVOS_4,
    PER_SERVOS_5,

    PER_SERVOS_MAX,
} PER_SERVOS_T;

typedef enum PER_LIGHTS_E {
    PER_LIGHTS_0 = 0,
    PER_LIGHTS_1,
    PER_LIGHTS_2,
    PER_LIGHTS_3,
    PER_LIGHTS_4,
    PER_LIGHTS_5,

    PER_LIGHTS_MAX,
} PER_LIGHTS_T;

/**
 * @brief Reserved for single instance peripherals.
 *
 */
typedef enum PER_SINGLE_E {
    PER_SINGLE,
} PER_SINGLE_E;

typedef union PER_INSTANCES_U {
    PER_SERVOS_T Servos;
    PER_LIGHTS_T Lights;
    PER_SINGLE_E Single;
} PER_INSTANCES_T;

typedef enum PER_TYPE_E {
    PER_TYPE_NONE = 0,  /**< Unused channel. */
    PER_TYPE_ABS_SERVO, /**< Servo with the angle directly controlled with the current RC set
                             point. */
    PER_TYPE_REL_SERVO, /**< Servo with the angle controlled based on the current angle and RC
                             set point. */
    PER_TYPE_LIGHT,     /**< To turn on and off the lights. */

    PER_TYPE_POWER_OFF, /**< To power off the board. */

    PER_TYPE_MAX,
} PER_TYPE_T;

typedef struct PER_PERIPHERAL_S {
    PER_TYPE_T      Type;
    PER_INSTANCES_T Instance;
} PER_PERIPHERAL_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t Per_Init(const PER_PERIPHERAL_T* p_per);
void   Per_Start(const PER_PERIPHERAL_T* p_per, float32_t initial_value);
void   Per_ApplySetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint);

#endif /* __PERIPHERALS_H__       */
