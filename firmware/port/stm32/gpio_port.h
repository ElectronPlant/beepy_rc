/**
 * @file  gpio_port.h
 * @brief STM32 specific defines for the GPIOs.
 *
 * @ingroup   GpioStm32
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __GPIO_PORT_H__
#define __GPIO_PORT_H__

#include "plt_types.h"

#include "hal_includes.h"


/** @addtogroup Port
 *   @{
 */

/** @addtogroup Gpio
 *   @{
 */

/** @addtogroup GpioStm32
 *   @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef struct {
    uint32_t      GpioPin;
    GPIO_TypeDef* GpioPort;
    uint32_t      GpioClk;
    void (*GpioClkEnFn_Ptr)(uint32_t);
} GPIO_PERIPHERAL_PORT_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/


/** @} (end addtogroup GpioStm32) */
/** @} (end addtogroup Gpio)      */
/** @} (end addtogroup Port)      */

#endif /* __GPIO_PORT_H__       */
