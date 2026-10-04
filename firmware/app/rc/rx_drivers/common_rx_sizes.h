/**
 * @file  common_rx_sizes.h
 * @brief Abstraction layer for the RX driver buffer and channel sizes.
 *
 * @ingroup   ComRxSizes
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __COMMON_RX_SIZES_H__
#define __COMMON_RX_SIZES_H__

#include "plt_types.h"
#include "target.h"


/** @addtogroup App
 *   @{
 */

/** @addtogroup Rc
 *   @{
 */

/** @addtogroup RxDriver
 *   @{
 */

/** @addtogroup ComRxSizes
 *   @{
 */

/********************************************************************************
 * Driver definitions
 ********************************************************************************/
#if TARGET_RC_DRIVER == SBUS
    #include "sbus.h"
    #define COMRXS_BUFFER_SIZE  (SBUS_FRAME_SIZE_BYTES)
    #define COMRXS_NUM_CHANNELS (SBUS_NUM_CHANNELS)
#else
    #error "Invalid RX Driver, review the TARGET_RX_DRIVER definition"
#endif

/********************************************************************************
 * Driver checks
 ********************************************************************************/
#if !defined(COMRXS_BUFFER_SIZE)
    #error "Missing buffer size for the selected driver"
#endif

#if !defined(COMRXS_NUM_CHANNELS)
    #error "Missing num channels for the selected driver"
#endif


/** @} (end addtogroup ComRxSizes)  */
/** @} (end addtogroup RxDriver)    */
/** @} (end addtogroup Rc)          */
/** @} (end addtogroup App)         */

#endif /* __COMMON_RX_SIZES_H__     */
