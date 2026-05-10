/**
 * @file      target.h
 * @brief     Target definition for the nucleo-F446 board.
 *
 * @ingroup   Target
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
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
/******************************************
 * Internal Components
 ******************************************/
/* --- LED --- */
#define TARGET_USE_LED
#define TARGET_LED_PIN        LL_GPIO_PIN_5
#define TARGET_LED_PORT       GPIOA
#define TARGET_LED_GPIO_CLOCK LL_AHB1_GRP1_PERIPH_GPIOA

/* --- Button --- */
/** @note List of notes:
 *      1. EXTI ints can be seen here:
 *         https://controllerstech.com/external-interrupt-using-registers/
 */
#define TARGET_USE_BUTTON
#define TARGET_BUTTON_PIN              LL_GPIO_PIN_13
#define TARGET_BUTTON_PORT             GPIOC
#define TARGET_BUTTON_GPIO_CLOCK       LL_AHB1_GRP1_PERIPH_GPIOC
#define TARGET_BUTTON_EXTI_LINE        LL_EXTI_LINE_13
#define TARGET_BUTTON_EXTI_IRQ         EXTI15_10_IRQn /* Note 1 */
#define TARGET_BUTTON_EXIT_IRQ_HANDLER EXTI15_10_IRQHandler
#define TARGET_BUTTON_SYSCFG_EXTI_PORT LL_SYSCFG_EXTI_PORTC
#define TARGET_BUTTON_SYSCFG_EXTI_LINE LL_SYSCFG_EXTI_LINE13

/* --- */ // TODO things I'm not sure I need
#if 0
    #define USART_TX_Pin       LL_GPIO_PIN_2
    #define USART_TX_GPIO_Port GPIOA
    #define USART_RX_Pin       LL_GPIO_PIN_3
    #define USART_RX_GPIO_Port GPIOA
    #define TMS_Pin            LL_GPIO_PIN_13
    #define TMS_GPIO_Port      GPIOA
    #define TCK_Pin            LL_GPIO_PIN_14
    #define TCK_GPIO_Port      GPIOA
    #define SWO_Pin            LL_GPIO_PIN_3
    #define SWO_GPIO_Port      GPIOB
#endif

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
/* Servo PWM timers */
extern PWM_TIM_INSTANCE_T Target_ServoPwmTimers[TARGET_NUM_SERVO_PWM_CHANNELS];
/* Servo Motors */
extern SERVO_T Target_Servos[TARGET_NUM_SERVO_PWM_CHANNELS];


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
