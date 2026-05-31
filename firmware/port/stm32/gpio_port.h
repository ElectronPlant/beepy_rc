/**
 * @file  gpio_port.h
 * @brief STM32 specific defines for the GPIOs.
 *
 * @ingroup   Stm32GpioPort
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __GPIO_PORT_H__
#define __GPIO_PORT_H__

#include "plt_types.h"

#include "hal_includes.h"


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


#endif /* __GPIO_PORT_H__       */
