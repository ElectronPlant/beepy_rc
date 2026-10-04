/**
 * @file     plt_assert.c
 * @brief    Assert implementation for the platform.
 *
 * @ingroup   PltAssert
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright (c) 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: PltAssert_
 *
 */

#include "stdio.h"

#include "plt_types.h"
#include "target.h"

/** @addtogroup Plt
 *   @{
 */


/** @addtogroup PltAssert
 *   @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/
void PltAssert_Assert(const char* file, uint32_t line) {
    printf("ASSERT: In %s, line %lu\n", file, line);
    // abort();
    __disable_irq();
    while (1) {
        /* --No-op-- */
    }
}

void PltAssert_WarningAssert(const char* file, uint32_t line) {
    printf("WARNING: In %s, line %lu\n", file, line);
}

/** @} (end addtogroup PltAssert)   */
/** @} (end addtogroup Plt)         */
