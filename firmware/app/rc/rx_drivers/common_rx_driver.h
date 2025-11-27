/**
 * @file  common_rx_driver.h
 * @brief Common implementations for the Rx drivers. Provides the abstraction layer for the
 *        different rx driver options.
 *
 * @ingroup   Rc
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __COMMON_RX_DRIVER_H__
#define __COMMON_RX_DRIVER_H__

#include "plt_types.h"
#include "target.h"


/********************************************************************************
 * Driver definitions
 ********************************************************************************/
#if TARGET_RC_DRIVER == SBUS
    #include "sbus.h"
    #define COMRXD_INTERFACE    Sbus_Interface
    #define COMRXD_BUFFER_SIZE  (SBUS_FRAME_SIZE_BYTES)
    #define COMRXD_NUM_CHANNELS (SBUS_NUM_CHANNELS)
#else
    #error "Invalid RX Driver, review the TARGET_RX_DRIVER definition"
#endif

/********************************************************************************
 * Driver checks
 ********************************************************************************/
#if !defined(COMRXD_INTERFACE)
    #error "Missing interface for the selected driver"
#endif

#if !defined(COMRXD_BUFFER_SIZE)
    #error "Missing buffer size for the selected driver"
#endif

#if !defined(COMRXD_NUM_CHANNELS)
    #error "Missing num channels for the selected driver"
#endif

/**
 * @brief  Ensures that all interfaces are correctly set.
 *         The check is done through static asserts, so it would not affect the resulting binary.
 */
static inline void ComRxD_CheckInterface(void) {
    PLT_BUILD_ASSERT(NULL != COMRXD_INTERFACE.RxInt_Init);
    PLT_BUILD_ASSERT(NULL != COMRXD_INTERFACE.RxInt_Start);
    PLT_BUILD_ASSERT(NULL != COMRXD_INTERFACE.RxInt_Stop);
    PLT_BUILD_ASSERT(NULL != COMRXD_INTERFACE.RxInt_PerformAlignment);
    PLT_BUILD_ASSERT(NULL != COMRXD_INTERFACE.RxInt_ProcessFrame);
    /* -- Debug frame is intentionally skipped in the check -- */
}


#endif /* __COMMON_RX_DRIVER_H__       */
