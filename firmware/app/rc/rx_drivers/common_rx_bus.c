/**
 * @file  common_rx_bus.c
 * @brief Abstraction layer for the Rx bus.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: ComRxBus_
 */

#include "plt_assert.h"
#include "plt_types.h"

#include "priorities_cfg.h"
#include "target.h"

#include "common_rx_bus.h"


/** @addtogroup Rc
 *   @{
 */

/** @addtogroup RxDriver
 *   @{
 */

/** @addtogroup ComRxBus
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
#if TARGET_RC_DRIVER == SBUS

    #include "serial.h"

static SERIAL_INSTANCE_T ComRxBus_BusInstance = {
    .Status = SERIAL_STATUS_UNINITIALIZED,
    .IrqPriority = PRIORITIES_CFG_IRQ_MAX_PRIORITY,
    .ErrorHandlerFunct_Ptr = NULL,
    .RxHandlerFunct_Ptr = NULL,
    .BaudRate = 0,
    .Peripheral = &TargetRcSerial
};

SERIAL_IRQ_CALLBACK(
    TARGET_RC_SERIAL_IRQ_HANDLER,
    ComRxBus_BusInstance.ErrorHandlerFunct_Ptr,
    ComRxBus_BusInstance.RxHandlerFunct_Ptr,
    ComRxBus_GetBusHandler
);

#else
    #error "Invalid common interface"
#endif

const COM_RX_BUS_HANDLER_T ComRxBus_BusHandler = &ComRxBus_BusInstance;

COM_RX_BUS_HANDLER_T ComRxBus_GetBusHandler(void) {
    return ComRxBus_BusHandler;
}


/** @} (end addtogroup ComRxBus)  */
/** @} (end addtogroup RxDriver)  */
/** @} (end addtogroup Rc)        */
