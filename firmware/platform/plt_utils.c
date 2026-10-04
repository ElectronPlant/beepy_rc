/**
 * @file  plt_utils.c
 * @brief Common utils implementation for the platform.
 *
 * @ingroup   Utils
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: PltUtils_
 */

#include "plt_assert.h"
#include "plt_types.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/** @addtogroup Plt
 *    @{
 */

/** @addtogroup PltUtils
 *    @{
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

/********************************************************************************
 * Bit operations
 ********************************************************************************/

/**
 * See header file.
 */
uint32_t PltUtils_GetFirstNonZeroOffset(uint32_t val) {
    uint32_t cnt;
    for (cnt = 0; cnt < 32U || (val >> cnt) != 0; cnt++) {
        /* - No-op - */
    }
    return cnt;
}


/********************************************************************************
 * Math functions
 ********************************************************************************/

/**
 * See header file.
 */
float32_t PltUtils_MapF32(
    float32_t v,
    float32_t in_min,
    float32_t in_max,
    float32_t out_min,
    float32_t out_max
) {
    return (((v - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min;
}

/********************************************************************************
 * FreeRTOS
 ********************************************************************************/
/**
 * See header file.
 * @note List of notes:
 *      1. This rounds the tick value once it is divided by the tick rate.
 */
uint32_t PltUtils_GetMillis(void) {
    TickType_t ticks = xTaskGetTickCount();
    ticks += (configTICK_RATE_HZ / 2); /* Note 1 */

    return ticks / configTICK_RATE_HZ;
}


/** @} (end addtogroup PltUtils)  */
/** @} (end addtogroup Plt)       */