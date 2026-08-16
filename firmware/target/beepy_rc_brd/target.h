/**
 * @file      target.h
 * @brief     Target definition for the BeepyRcBrd board.
 *
 * @ingroup   Target
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __TARGET_H__
#define __TARGET_H__

#include "hal_includes.h"

/* BSP */
#include "bsp.h"

/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Rc
 ********************************************************************************/
/* RC Input */
#define TARGET_RC_DRIVER SBUS

/* UART port used for SBUS */
#include "serial_port.h"
#define TARGET_RC_SERIAL_IRQ_HANDLER UART4_IRQHandler
extern const SERIAL_PERIPHERAL_PORT_T TargetRcSerial;


/********************************************************************************
 * Drive
 ********************************************************************************/
#define TARGET_VEHICLE_TYPE FWD

/**
 * Motor PWM timers
 * Each motor is driven by two PWM outputs.
 */
#include "pwm_timer_port.h"
#define TARGET_NUM_MOTOR_PWM_TIMERS (3U)
/* Motor PWM timers are defined  in the target.c file. */
extern const PWM_TIM_PORT_T TargetMotorTim1, TargetMotorTim2, TargetMotorTim3;


/**
 * Encoder timers
 * @note List of notes:
 *      1. At the moment is not clear if 4 or 2 encoders will be used. In the
 *         future it may be something adjustable from the target definition,
 *         but for development speed, 4 encoders will be presumed for now.
 */
#include "encoder_port.h"
#define TARGET_ENCODER_NUM (4U)
/* Encoders are defined in the target.c file. */
extern const ENC_PERIPHERAL_PORT_T TargetEnc1, TargetEnc2, TargetEnc3, TargetEnc4;

/**
 * Motors
 * Each motor is composed from two PWM outputs, and a quadrature encoder.
 */
#include "motor.h"
#define TARGET_MOTOR_NUM (4U)
extern MOTOR_T Target_Motors[TARGET_MOTOR_NUM];

/**
 * Servo motor PWM timers
 * Each servo motor is driven by a single PWM output.
 */
#include "pwm_timer_port.h"
#include "servo.h"
#define TARGET_NUM_SERVO_PWM_TIMERS   (2U)
#define TARGET_NUM_SERVO_PWM_CHANNELS (3U)
#define TARGET_NUM_SERVOS             (TARGET_NUM_SERVO_PWM_CHANNELS)
/* Servo Motors */
extern SERVO_HANDLER_T Target_Servos[TARGET_NUM_SERVOS];


/********************************************************************************
 * UI
 ********************************************************************************/
#include "gpio_port.h"

#include "button.h"
#include "gpio.h"

/* --- LEDs --- */
#define TARGET_NUM_LEDS (3U)
extern GPIO_HANDLER_T Target_Leds[TARGET_NUM_LEDS];

/* --- Buttons --- */
#define TARGET_NUM_BUTTONS (2U)
extern BUTTON_HANDLER_T Target_Buttons[TARGET_NUM_BUTTONS];

/* --- Battery --- */
extern GPIO_HANDLER_T Target_BatEnable;

/********************************************************************************
 * Lights
 ********************************************************************************/
/* TODO no support for lights at the moment */
#define TARGET_NUM_LIGHTS (0U)
extern GPIO_HANDLER_T Target_Lights[1U];

/********************************************************************************
 * Control Inputs
 ********************************************************************************/
#include "peripherals.h"
#include "rc_subs.h"

extern const RCSUBS_DRIVE_INPUTS_T Target_DriveSubs[RCSUBS_DRIVE_SETPOINT_MAX];

#define TARGET_NUM_AUX_PERIPHERALS (TARGET_NUM_SERVOS + TARGET_NUM_LIGHTS)
extern const RCSUBS_AUX_INPUTS_T Target_AuxSubs[TARGET_NUM_AUX_PERIPHERALS];


/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * API
 ********************************************************************************/


#endif /* __TARGET_H__       */
