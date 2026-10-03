/**
 * @file  curves.c
 * @brief Library with curves to map RC channels to control values.
 *
 * @ingroup   Curves
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Curves_
 */

#include "plt_assert.h"
#include "plt_types.h"

#include "curves.h"
#include "std_frame.h"


/** @addtogroup Controller
 *    @{
 */

/** @addtogroup Curves
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef void (*CURVES_APPLY_FUNCT)(const CURVES_T* p_curve, float32_t input, float32_t* p_output);


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
void Curves_ApplyNone(const CURVES_T* p_curve, float32_t input, float32_t* p_output);
void Curves_ApplyLinear(const CURVES_T* p_curve, float32_t input, float32_t* p_output);
void Curves_ApplyLinearWithDeadband(const CURVES_T* p_curve, float32_t input, float32_t* p_output);
void Curves_ApplyThreshold(const CURVES_T* p_curve, float32_t input, float32_t* p_output);
void Curves_ApplyInvThreshold(const CURVES_T* p_curve, float32_t input, float32_t* p_output);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
const CURVES_APPLY_FUNCT CurveApplyFuncs[CURVES_NAME_MAX] = {
    Curves_ApplyNone,
    Curves_ApplyLinear,
    Curves_ApplyLinearWithDeadband,
    Curves_ApplyThreshold,
    Curves_ApplyInvThreshold
};

/********************************************************************************
 * Function Implementations
 ********************************************************************************/
/**
 * @brief  Apply none curve.
 *
 * @param  p_curve  Pointer to the curve to apply.
 * @param  input    Value for which to apply the curve.
 * @param  p_output Pointer to where the curve adjusted value will be stored.
 */
void Curves_ApplyNone(const CURVES_T* p_curve, float32_t input, float32_t* p_output) {
    *p_output = input;
}

/**
 * @brief  Apply linear curve.
 *
 * @param  p_curve  Pointer to the curve to apply.
 * @param  input    Value for which to apply the curve.
 * @param  p_output Pointer to where the curve adjusted value will be stored.
 */
void Curves_ApplyLinear(const CURVES_T* p_curve, float32_t input, float32_t* p_output) {
    float temp = (p_curve->Params[0] + input) * p_curve->Params[1];
    *p_output = PLT_UTILS_SATURATE(temp, STD_FRAME_LOW_LIMIT, STD_FRAME_HIGH_LIMIT);
}

/**
 * @brief  Apply linear curve with deadband.
 *
 * @param  p_curve  Pointer to the curve to apply.
 * @param  input    Value for which to apply the curve.
 * @param  p_output Pointer to where the curve adjusted value will be stored.
 */
void Curves_ApplyLinearWithDeadband(const CURVES_T* p_curve, float32_t input, float32_t* p_output) {
    float32_t temp = 0.0f;
    if (p_curve->Params[2] < input || p_curve->Params[3] > input) {
        Curves_ApplyLinear(p_curve, input, &temp);
    } else {
        temp = p_curve->Params[0];
    }
    *p_output = temp;
}

/**
 * @brief  Apply threshold curve.
 *
 * @param  p_curve  Pointer to the curve to apply.
 * @param  input    Value for which to apply the curve.
 * @param  p_output Pointer to where the curve adjusted value will be stored.
 */
void Curves_ApplyThreshold(const CURVES_T* p_curve, float32_t input, float32_t* p_output) {
    *p_output = input > p_curve->Params[0] ? STD_FRAME_HIGH_LIMIT : STD_FRAME_LOW_LIMIT;
}

/**
 * @brief  Apply inverted threshold curve.
 *
 * @param  p_curve  Pointer to the curve to apply.
 * @param  input    Value for which to apply the curve.
 * @param  p_output Pointer to where the curve adjusted value will be stored.
 */
void Curves_ApplyInvThreshold(const CURVES_T* p_curve, float32_t input, float32_t* p_output) {
    *p_output = input < p_curve->Params[0] ? STD_FRAME_HIGH_LIMIT : STD_FRAME_LOW_LIMIT;
}

/**
 * @brief  Apply curve.
 *
 * @param  p_curve  Pointer to the curve to apply.
 * @param  input    Value for which to apply the curve.
 * @param  p_output Pointer to where the curve adjusted value will be stored.
 */
void Curves_ApplyCurve(const CURVES_T* p_curve, float32_t input, float32_t* p_output) {
    PLT_ASSERT(CURVES_NAME_MAX >= p_curve->Name);
    CurveApplyFuncs[p_curve->Name](p_curve, input, p_output);
}

/**
 * @brief  Translates a channel from analog representation to digital.
 *
 * @param  v Value to translate.
 *
 * @return Translated value, see note 1.
 *
 * @note List of notes:
 *       1. The output is considered DEF_TRUE if the input is greater or equal to 1.0f; otherwise
 *          is set to DEF_FALSE.
 */
bool_t Curves_Analog2Dig(float32_t v) {
    return 0.0f <= v ? DEF_TRUE : DEF_FALSE;
}


/** @} (end addtogroup Curves)  */
/** @} (end addtogroup Controller)  */
