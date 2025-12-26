/**
 * @file  pwm_timer_port.h
 * @brief STM32 specific defines for the PWM timer channels.
 *
 * @ingroup   DrivePwmTimerPort
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PWM_TIMER_PORT_H__
#define __PWM_TIMER_PORT_H__

#include "plt_types.h"

#include "hal_includes.h"


/********************************************************************************
 * Defines
 ********************************************************************************/
#define PWM_TIM_PORT_MAX_NUM_CHANNELS (4)

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef struct {
    uint32_t      GpioPin;
    GPIO_TypeDef* GpioPort;
    uint32_t      GpioAlternateFunc;
    uint32_t      GpioClk;
    void (*GpioClkEnFn_Ptr)(uint32_t);
    uint32_t OutputPolarity;
    uint32_t OutputIdleState;
} PWM_TIM_PORT_CHN_T;

typedef struct {
    TIM_TypeDef* Timer;
    uint32_t     TimerClk;
    void (*TimerClkEnFn_Ptr)(uint32_t);
    uint32_t TimerPrescaller;
    uint32_t TimerClkDivision;
    uint32_t TimerClkFreqKhz;
    bool_t   TimerIs32bits;
    uint8_t  NumChannels;

    const PWM_TIM_PORT_CHN_T* Channels[PWM_TIM_PORT_MAX_NUM_CHANNELS];
} PWM_TIM_PORT_T;

#endif /* __PWM_TIMER_PORT_H__       */
