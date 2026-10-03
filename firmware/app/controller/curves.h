/**
 * @file  curves.h
 * @brief Library with curves to map RC channels to control values.
 *
 * @ingroup   Curves
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __CURVES_H__
#define __CURVES_H__

#include "plt_types.h"


/********************************************************************************
 * Defines
 ********************************************************************************/
#define CURVES_MAX_PARAMS (5U)


/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum CURVES_NAME_E {
    /* --- Analog Curves --- */
    CURVES_NAME_NONE = 0,             /**< Keep output as is. y = x */
    CURVES_NAME_LINEAR,               /**< First order polynomial mapping.  y = (p[0] + x) * p[1] */
    CURVES_NAME_LINEAR_WITH_DEADBAND, /**< Linear curve with deadband.
                                            y = {x > p[2]: (p[0] + (x - p[2])) * p[1]
                                                 x < p[3]: (p[0] + (x + p[3])) * p[1]
                                                 else 0 } */

    /* --- Digital Curves --- */
    CURVES_NAME_THRESHOLD,     /**< STD_FRAME_HIGH_LIMIT if higher than p[0],
                                    STD_FRAME_LOW_LIMIT otherwise. */
    CURVES_NAME_INV_THRESHOLD, /**< STD_FRAME_HIGH_LIMIT if lower than p[0],
                                    STD_FRAME_LOW_LIMIT otherwise. */

    CURVES_NAME_MAX, /**< __INVALID__ sentinel value marking the end of the curves. */
} CURVES_NAME_T;


typedef struct CURVES_S {
    CURVES_NAME_T Name;
    float32_t     Params[CURVES_MAX_PARAMS];
} CURVES_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
void   Curves_ApplyCurve(const CURVES_T* p_curve, float32_t input, float32_t* p_output);
bool_t Curves_Analog2Dig(float32_t v);

#endif /* __CURVES_H__       */
