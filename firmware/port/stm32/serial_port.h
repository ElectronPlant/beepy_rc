/**
 * @file  encoder_port.h
 * @brief STM32 specific defines for the serial interface.
 *
 * @ingroup   SerialStm32
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include "plt_types.h"

#include "hal_includes.h"


/** @addtogroup Port
 *    @{
 */

/** @addtogroup Serial
 *    @{
 */

/** @addtogroup SerialStm32
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef struct {
    USART_TypeDef* Serial;
    uint32_t       Clk;
    void (*ClkEnFn_Ptr)(uint32_t);

    IRQn_Type IrqType;

    bool_t        TxAvailable;
    uint32_t      TxPin;
    GPIO_TypeDef* TxPort;
    uint32_t      TxAlternateFunc;
    uint32_t      TxClk;
    void (*TxClkEnFn_Ptr)(uint32_t);

    bool_t        RxAvailable;
    uint32_t      RxPin;
    GPIO_TypeDef* RxPort;
    uint32_t      RxAlternateFunc;
    uint32_t      RxClk;
    void (*RxClkEnFn_Ptr)(uint32_t);
} SERIAL_PERIPHERAL_PORT_T;


/** @} (end addtogroup SerialStm32) */
/** @} (end addtogroup Serial)      */
/** @} (end addtogroup Port)        */

#endif /* __SERIAL_PORT_H__       */
