/**
 * @file     plt_utils.h
 * @brief    Collection of common utilities for the platform.
 *           This is intended to include all the things that may be required extensively through
 *           the project, but cannot really fit in a different module.
 *
 * @ingroup   PltUtils
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright (c) 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PLT_UTILS_H__
#define __PLT_UTILS_H__

#include "plt_defines.h"
#include "plt_types.h"

#if PLT_DEFINES_USE_FREE_RTOS == 1
    #include "FreeRTOS.h"
    #include "task.h"
#endif /* USE_FREE_RTOS == 1 */

/** \addtogroup PltUtils
 *   @{
 */

/********************************************************************************
 * Attributes
 ********************************************************************************/

/**
 * @brief Attribute for the unused variables.
 *
 * @note Usage example where the x variable will not be used:
 *       ```
 *       static void TestFunc(uint8_t PLT_UTILS_UNUSED x, uint8_t y)
 *       ```
 */
#define PLT_UTILS_UNUSED __attribute__((unused))

/**
 * @brief Attribute to pack structs.
 *
 * @note Usage example where the x variable will not be used:
 *       ```
 *       typedef struct {
 *              a : 1; // with 1 being the size in bits of the a variable.
 *       } PLT_UTILS_PACKED TEST_STUCT_T;
 *       ```
 */
#define PLT_UTILS_PACKED __attribute__((__packed__))

/********************************************************************************
 * Bit operations
 ********************************************************************************/
/**
 * @brief  Sets the mask for a bit based on its offset.
 *
 * @param  X offset of the bit for which to get the mask.
 */
#define PLT_UTILS_BIT_OFFSET_TO_MASK(X) (0x01 << X)

/**
 * @brief  Checks if the bit specified by the offset is set or not.
 *
 * @param  Y Bit map.
 * @param  X offset of the bit for which to get the mask.
 */
#define PLT_UTILS_IS_BIT_OFFSET_SET(Y, X) ((Y && (0x01 << X)) == 0 ? DEF_FALSE : DEF_TRUE)

/**
 * @brief  Returns the offset of the first non-zero bit.
 *
 * @param  val Value to check.
 *
 * @return Offset of the first non-zero bit 32 if no offset was found.
 */
uint32_t PltUtils_GetFirstNonZeroOffset(uint32_t val);


/********************************************************************************
 * BASIC MATH
 ********************************************************************************/
/**
 * @brief  Returns the maximum between A and B.
 *
 * @param A: First value.
 * @param B: Second value
 *
 * @return MAX(A, B).
 */
#define PLT_UTILS_MAX(A, B) ((A) > (B) ? (A) : (B))

/**
 * @brief  Returns the minimum between A and B.
 *
 * @param A: First value.
 * @param B: Second value
 *
 * @return MIN(A, B).
 */
#define PLT_UTILS_MIN(A, B) ((A) < (B) ? (A) : (B))

/**
 * @brief  Absolute value of A.
 *
 * @param  A: Value for which to get the absolute.
 *
 * @return ABS(A).
 */
#define PLT_UTILS_ABS(A) ((A) < 0 ? -(A) : (A))

/**
 * @brief  Saturates a value to the defined range.
 *
 * @param  A: Value to saturate.
 * @param  L_MIN: Lower limit of the range.
 * @param  L_MAX: Higher limit of the range.
 *
 * @return A saturated to the defined range.
 */
#define PLT_UTILS_SATURATE(A, L_MIN, L_MAX) (PLT_UTILS_MAX(PLT_UTILS_MIN((A), (L_MAX)), (L_MIN)))


/********************************************************************************
 * CONVERSION
 ********************************************************************************/
#define PLT_UTILS_SECS_TO_MS_FACTOR (1000u)
/**
 * @brief  Converts a uint32_t value in seconds to MS.
 *
 * @param  X Value in seconds to convert.
 *
 * @return X param in ms.
 *
 * @note  This macro does not perform any overflow check.
 */
#define PLT_UTILS_SECS_TO_MS(X) ((X) * PLT_UTILS_SECS_TO_MS_FACTOR)

/**
 * @brief  Rounds a float32_t value to the closest integer value.
 *         Note that it does not change for ranges.
 *
 * @param  X Float value to convert.
 *
 * @return Rounded X as integer.
 */
#define PLT_UTILS_ROUND_FLOAT(X) ((int32_t)(X >= 0.0f ? X + 0.5f : X - 0.5f))

/**
 * @brief  Rounds a positive float32_t value to the closest unsigned integer value.
 *         Note that it does not change for ranges.
 *
 * @param  X Float value to convert.
 *
 * @return Rounded X as unsigned integer.
 */
#define PLT_UTILS_ROUND_FLOAT_TO_UINT(X) ((uint32_t)(X >= 0.0f ? X + 0.5f : X - 0.5f))

/********************************************************************************
 * Math
 ********************************************************************************/
/**
 * @brief Re-scale a float32_t value from one range to another.
 *        This function is used to calculate the equivalent value in a different range.
 *        This is a similar function to the Map() function used in Arduino. See note 1.
 *
 * @param  v Value to rescale.
 * @param  in_min Low limit of the input range.
 * @param  in_max High limit of the input range.
 * @param  out_min Low limit of the output range
 * @param  out_max High limit of the output range.
 *
 * @return Re-scaled value.
 *
 * @note List of notes:
 *       1. This function does not check for out of range values.
 */
float32_t PltUtils_MapF32(
    float32_t v,
    float32_t in_min,
    float32_t in_max,
    float32_t out_min,
    float32_t out_max
);

/********************************************************************************
 * FREERTOS
 ********************************************************************************/
#if PLT_DEFINES_USE_FREE_RTOS == 1

    /**
 * @brief  Converts Basetype pdPASS/pdFAIL to DEF_TRUE/DEF_FALSE.
 *
 * @param  X FreeRTOS pass fail value.
 *
 * @return DEF_TRUE if pdPASS, DEF_FALSE otherwise.
 */
    #define PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(X) (pdTRUE == X ? DEF_TRUE : DEF_FALSE)

/**
 * @brief Gets the time in ms since the last reboot.
 *        Warning, this resets every 49 days, so it should only be used for relative time
 *        calculations.
 *
 * @return Time in ms since the last reboot.
 */
uint32_t PltUtils_GetMillis(void);

#endif /* USE_FREE_RTOS == 1 */
/********************************************************************************
 * STM_LL
 ********************************************************************************/
/**
 * @brief  Converts STM LL HAL ErrorStatus to DEF_TRUE/DEF_FALSE.
 *
 * @param  X STM LL HAL ErrorStatus.
 *
 * @return DEF_TRUE if SUCCESS, DEF_FALSE otherwise.
 */
#define PLT_UTILS_STM_ERR_STATUS_TO_PLT(X) (SUCCESS == X ? DEF_TRUE : DEF_FALSE)

/** @} (end addtogroup PltUtils)  */
#endif /* __PLT_UTILS_H__       */
