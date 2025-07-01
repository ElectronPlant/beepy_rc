/**
 * @file     plt_types.h
 * @brief    Default types of the platform.
 *
 * @ingroup   PltTypes
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright (c) 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PLT_TYPES_H__
#define __PLT_TYPES_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/** \addtogroup PltTypes
 *   @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
/** Bool defines */
#define DEF_TRUE  1u
#define DEF_FALSE 0u

/********************************************************************************
 * Typedefs
 ********************************************************************************/
/** Floating point types */
typedef float  float32_t;
typedef double float64_t;

/** Bool type    */
typedef uint8_t bool_t;


/** @} (end addtogroup PltTypes)  */

#endif /* __PLT_TYPES_H__      */
