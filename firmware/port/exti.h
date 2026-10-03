/**
 * @file  exti.h
 * @brief External Interrupts.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __EXTI_H__
#define __EXTI_H__

#include "plt_types.h"

#include "gpio.h"


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/**
 * @brief Type definition for the EXTI callback functions.
 */
typedef void (*EXTI_CALLBACK_FUNC)(void*);

typedef enum EXTI_MODE_E {
    EXTI_MODE_BOTH = 0,
    EXTI_MODE_RISING,
    EXTI_MODE_FALLING,
} EXTI_MODE_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
/**
 * @brief  Registers a callback to the specified GPIO.
 *
 * @param  gpio Handler of the GPIO for which to register the interrupt.
 * @param  callback_prt Pointer to the callback function for the ISR.
 * @param  mode Interrupt mode.
 * @param  p_args Pointer to the arguments to be passed to the callback.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Exti_Enable(
    GPIO_HANDLER_T     gpio,
    EXTI_CALLBACK_FUNC callback_ptr,
    EXTI_MODE_T        mode,
    void*              p_args
);

/**
 * @brief  Disables the interrupt registered to the specified GPIO.
 *
 * @param  gpio Handler of the GPIO for which to disable the interrupt.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Exti_Disable(GPIO_HANDLER_T gpio);

#endif /* __EXTI_H__       */
