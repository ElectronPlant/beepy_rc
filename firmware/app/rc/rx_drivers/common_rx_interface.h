/**
 * @file  common_rx_interface.h
 * @brief Abstraction layer for the RX driver interface.
 *
 * @ingroup   Rc
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __COMMON_RX_INT_H__
#define __COMMON_RX_INT_H__

#include "plt_types.h"

#include "rx_interface.h"
#include "target.h"


/********************************************************************************
 * Driver definitions
 ********************************************************************************/
#if TARGET_RC_DRIVER == SBUS

    #define RXINT_BUS SERIAL

bool_t Sbus_Init(
    COM_RX_BUS_HANDLER_T h_ser,
    RxInt_RxHandler      rx_handler_func,
    RxInt_RxErrorHandler rx_error_func
);
bool_t Sbus_Start(COM_RX_BUS_HANDLER_T h_ser);
void   Sbus_Stop(COM_RX_BUS_HANDLER_T h_ser);
void   Sbus_PerformAlignment(RXINT_RX_INFO_T* p_rx_info, uint8_t rx_byte);
void   Sbus_ProcessFrame(RXINT_RX_BUFFER_INFO_T* p_buffer_info, STD_FRAME_T* p_std_frame);
void   Sbus_DebugFrame(RXINT_RX_BUFFER_INFO_T* p_buffer_info);

const RXINT_INTERFACE_T ComRxInt_Interface = {
    .RxInt_Init = Sbus_Init,
    .RxInt_Start = Sbus_Start,
    .RxInt_Stop = Sbus_Stop,
    .RxInt_PerformAlignment = Sbus_PerformAlignment,
    .RxInt_ProcessFrame = Sbus_ProcessFrame,
    .RxInt_DebugFrame = Sbus_DebugFrame
};

#else
    #error "Invalid RX Driver, review the TARGET_RX_DRIVER definition"
#endif


/********************************************************************************
 * Driver checks
 ********************************************************************************/
/**
 * @brief  Ensures that all interfaces are correctly set.
 *         The check is done through static asserts, so it would not affect the resulting binary.
 */
static inline void ComRxInt_CheckInterface(void) {
    PLT_BUILD_ASSERT(NULL != ComRxInt_Interface.RxInt_Init);
    PLT_BUILD_ASSERT(NULL != ComRxInt_Interface.RxInt_Start);
    PLT_BUILD_ASSERT(NULL != ComRxInt_Interface.RxInt_Stop);
    PLT_BUILD_ASSERT(NULL != ComRxInt_Interface.RxInt_PerformAlignment);
    PLT_BUILD_ASSERT(NULL != ComRxInt_Interface.RxInt_ProcessFrame);
    /* -- Debug frame is intentionally skipped in the check -- */
}


#endif /* __COMMON_RX_INT_H__       */
