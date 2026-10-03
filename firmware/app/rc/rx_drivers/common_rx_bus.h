/**
 * @file  common_rx_bus.h
 * @brief Abstraction layer for the RX driver bus.
 *
 * @ingroup   Rc
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __COMMON_RX_BUS_H__
#define __COMMON_RX_BUS_H__

#include "plt_types.h"
#include "target.h"


/********************************************************************************
 * Bus Handler
 ********************************************************************************/
#if TARGET_RC_DRIVER == SBUS

    #include "serial.h"
typedef SERIAL_HANDLER_T COM_RX_BUS_HANDLER_T;

#else
    #error "Invalid common interface"
#endif

extern const COM_RX_BUS_HANDLER_T ComRxBus_BusHandler;
COM_RX_BUS_HANDLER_T              ComRxBus_GetBusHandler(void);


#endif /* __COMMON_RX_BUS_H__       */
