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
 * @brief Converts a uint32_t value in seconds to MS.
 *
 * @note  This macro does not perform any overflow check.
 */
#define PLT_UTILS_SECS_TO_MS(X) ((X) * 1000u)


/** @} (end addtogroup PltUtils)  */
#endif /* __PLT_UTILS_H__       */
