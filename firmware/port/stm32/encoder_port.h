/**
 * @file  encoder_port.h
 * @brief STM32 specific defines for the encoders.
 *
 * @ingroup   EncoderStm32
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __ENCODER_PORT_H__
#define __ENCODER_PORT_H__

#include "plt_types.h"

#include "hal_includes.h"


/** @addtogroup Port
 *   @{
 */

/** @addtogroup Encoder
 *   @{
 */

/** @addtogroup EncoderStm32
 *   @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef struct {
    TIM_TypeDef* Timer;
    uint32_t     TimerClk;
    void (*TimerClkEnFn_Ptr)(uint32_t);
    uint32_t TimerChn1;
    uint32_t TimerChn2;

    uint32_t      Gpio1Pin;
    GPIO_TypeDef* Gpio1Port;
    uint32_t      Gpio1AlternateFunc;
    uint32_t      Gpio1Clk;
    void (*Gpio1ClkEnFn_Ptr)(uint32_t);

    uint32_t      Gpio2Pin;
    GPIO_TypeDef* Gpio2Port;
    uint32_t      Gpio2AlternateFunc;
    uint32_t      Gpio2Clk;
    void (*Gpio2ClkEnFn_Ptr)(uint32_t);
} ENC_PERIPHERAL_PORT_T;


/** @} (end addtogroup EncoderStm32) */
/** @} (end addtogroup Encoder)      */
/** @} (end addtogroup Port)         */

#endif /* __ENCODER_PORT_H__         */
