/**
 * @file  curves.h
 * @brief Library for the control curves.
 *
 * @ingroup   Curves
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __CURVES_H__
#define __CURVES_H__

#include "plt_types.h"


/********************************************************************************
 * Defines
 ********************************************************************************/
#define CURVE_MAX_STATES (5U)


/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum CURVE_TYPE_E {
    CURVE_TYPE_NONE = 0,
    CURVE_DIGITAL,
    CURVE_TYPE_STATES,
    CURVE_TYPE_ANALOG,

    CURVE_TYPE_MAX,
} CURVE_TYPE_T;

typedef struct CURVE_PARAMS_DIGITAL_S {
    float32_t Threshold;
} CURVE_PARAMS_DIGITAL_T;

typedef struct CURVE_PARAMS_STATES_S {
    float32_t Thresholds[CURVE_MAX_STATES];
} CURVE_PARAMS_STATES_T;

typedef struct CURVE_PARAMS_ANALOG_S {
    float32_t Slope;
    float32_t Gain;
} CURVE_PARAMS_ANALOG_T;

typedef union CURVE_PARAMS_U {
    CURVE_PARAMS_DIGITAL_T Digital;
    CURVE_PARAMS_STATES_T  States;
    CURVE_PARAMS_ANALOG_T  Analog;
} CURVE_PARAMS_T;

typedef struct CURVE_S {
    CURVE_TYPE_T   Type;
    CURVE_PARAMS_T Param;
} CURVE_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/


#endif /* __CURVES_H__       */
