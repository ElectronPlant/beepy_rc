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

#include "FreeRTOS.h"
#include "plt_types.h"


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
 * CONVERSION
 ********************************************************************************/
/**
 * @brief  Converts a uint32_t value in seconds to MS.
 *
 * @param  X Value in seconds to convert.
 *
 * @return X param in ms.
 *
 * @note  This macro does not perform any overflow check.
 */
#define PLT_UTILS_SECS_TO_MS(X) ((X) * 1000u)

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
 * FREERTOS
 ********************************************************************************/
/**
 * @brief  Converts Basetype pdPASS/pdFAIL to DEF_TRUE/DEF_FALSE.
 *
 * @param  X FreeRTOS pass fail value.
 *
 * @return DEF_TRUE if pdPASS, DEF_FALSE otherwise.
 */
#define PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(X) (pdTRUE == X ? DEF_TRUE : DEF_FALSE)

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
