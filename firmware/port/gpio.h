/**
 * @file  gpio.h
 * @brief Driver for the GPIOs.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "plt_types.h"


/********************************************************************************
 * Defines
 ********************************************************************************/
#define GPIO_BOOL_TO_VALUE(X) (X == DEF_TRUE ? GPIO_VALUE_HIGH : GPIO_VALUE_LOW)
#define GPIO_VALUE_TO_BOOL(X) (X == GPIO_VALUE_HIGH ? DEF_TRUE : DEF_FALSE)

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum GPIO_MODE_E {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT,
} GPIO_MODE_T;

typedef enum GPIO_VALUE_E {
    GPIO_VALUE_LOW = 0,
    GPIO_VALUE_HIGH,
} GPIO_VALUE_T;

typedef enum GPIO_PULL_E {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_DOWN,
    GPIO_PULL_UP,
} GPIO_PULL_T;

typedef enum GPIO_STATUS_E {
    GPIO_STATUS_UNINITIALIZED = 0,
    GPIO_STATUS_INITIALIZED,
    GPIO_STATUS_RUNNING,
} GPIO_STATUS_T;

typedef const GPIO_PERIPHERAL_PORT_T* GPIO_PERIPHERAL_T;

typedef struct GPIO_INSTANCE_S {
    GPIO_STATUS_T           Status; /* Must be set to GPIO_STATUS_UNINITIALIZED on the struct def */
    GPIO_MODE_T             Mode;
    GPIO_PULL_T             Pull;
    const GPIO_PERIPHERAL_T Peripheral_Ptr;
} GPIO_INSTANCE_T;

typedef GPIO_INSTANCE_T* GPIO_HANDLER_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
/**
 * @brief  Initializes the GPIO. See note 1.
 *
 * @param  gpio Gpio handler.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. The gpio instance must be uninitialized before being uninitialized.
 */
bool_t Gpio_Init(GPIO_HANDLER_T gpio);

/**
 * @brief  Sets the output of the specified GPIO.
 *
 * @param  gpio GPIO handler.
 * @param  value Value to set.
 */
void Gpio_Write(GPIO_HANDLER_T gpio, GPIO_VALUE_T value);

/**
 * @brief  Toggles the output of the specified GPIO.
 *
 * @param  gpio GPIO handler.
 */
void Gpio_Toggle(GPIO_HANDLER_T gpio);

/**
 * @brief  Reads the output of the specified GPIO.
 *
 * @param  gpio GPIO handler.
 *
 * @return Read value.
 */
GPIO_VALUE_T Gpio_Read(GPIO_HANDLER_T gpio);


#endif /* __GPIO_H__       */
