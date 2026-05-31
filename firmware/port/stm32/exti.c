/**
 * @file  exti.c
 * @brief STM port for the external interrupts.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Todo_
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "stm32f4xx_ll_exti.h"

#include "gpio_port.h"

#include "exti.h"
#include "gpio.h"


/** @addtogroup Ports
 *    @{
 */

/** @addtogroup EXTI
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define EXTI_MAX_INTERRUPT_LINES (15U)


/********************************************************************************
 * Typedefs
 ********************************************************************************/


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/
static EXTI_CALLBACK_FUNC Exti_Callbacks[EXTI_MAX_INTERRUPT_LINES] = {NULL};
static void*              Exti_Args[EXTI_MAX_INTERRUPT_LINES] = {NULL};

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

bool_t Exti_GpioToExtiLine(uint32_t gpio, uint32_t* p_exti) {
    bool_t ok = DEF_TRUE;

    switch (gpio) {
        case LL_GPIO_PIN_0:
            *p_exti = LL_EXTI_LINE_0;
            break;
        case LL_GPIO_PIN_1:
            *p_exti = LL_EXTI_LINE_1;
            break;
        case LL_GPIO_PIN_2:
            *p_exti = LL_EXTI_LINE_2;
            break;
        case LL_GPIO_PIN_3:
            *p_exti = LL_EXTI_LINE_3;
            break;
        case LL_GPIO_PIN_4:
            *p_exti = LL_EXTI_LINE_4;
            break;
        case LL_GPIO_PIN_5:
            *p_exti = LL_EXTI_LINE_5;
            break;
        case LL_GPIO_PIN_6:
            *p_exti = LL_EXTI_LINE_6;
            break;
        case LL_GPIO_PIN_7:
            *p_exti = LL_EXTI_LINE_7;
            break;
        case LL_GPIO_PIN_8:
            *p_exti = LL_EXTI_LINE_8;
            break;
        case LL_GPIO_PIN_9:
            *p_exti = LL_EXTI_LINE_9;
            break;
        case LL_GPIO_PIN_10:
            *p_exti = LL_EXTI_LINE_10;
            break;
        case LL_GPIO_PIN_11:
            *p_exti = LL_EXTI_LINE_11;
            break;
        case LL_GPIO_PIN_12:
            *p_exti = LL_EXTI_LINE_12;
            break;
        case LL_GPIO_PIN_13:
            *p_exti = LL_EXTI_LINE_13;
            break;
        case LL_GPIO_PIN_14:
            *p_exti = LL_EXTI_LINE_14;
            break;
        case LL_GPIO_PIN_15:
            *p_exti = LL_EXTI_LINE_15;
            break;
        default:
            ok = DEF_FALSE;
            break;
    }
    return ok;
}

bool_t Exti_EnableMode(EXTI_MODE_T mode, uint32_t line) {
    bool_t ok = DEF_TRUE;

    switch (mode) {
        case EXTI_MODE_BOTH:
            LL_EXTI_EnableIT_0_31(line);
            break;
        case EXTI_MODE_RISING:
            LL_EXTI_EnableRisingTrig_0_31(line);
            break;
        case EXTI_MODE_FALLING:
            LL_EXTI_EnableFallingTrig_0_31(line);
            break;
        default:
            ok = DEF_FALSE;
            break;
    }
    return ok;
}

bool_t Exti_Enable(
    GPIO_HANDLER_T     gpio,
    EXTI_CALLBACK_FUNC callback,
    EXTI_MODE_T        mode,
    void*              p_arg
) {
    bool_t   ok = DEF_TRUE;
    uint32_t line;

    ok = Exti_GpioToExtiLine(gpio->Peripheral_Ptr->GpioPin, &line);

    if (DEF_TRUE == ok) {
        /* Ensure that the GPIO has the correct mode, the new callback is not NULL and that
           there are no prior interrupts registered to that line. */
        if (GPIO_MODE_INPUT != gpio->Mode || NULL == callback || NULL != Exti_Callbacks[line]) {
            ok = DEF_FALSE;
        }
    }

    if (DEF_TRUE == ok) {
        Exti_Callbacks[line] = callback;
        Exti_Args[line] = p_arg;

        Exti_EnableMode(mode, line);
        LL_EXTI_ClearFlag_0_31(line);
    }
    return ok;
}

bool_t Exti_Disable(GPIO_HANDLER_T gpio) {
    bool_t   ok = DEF_TRUE;
    uint32_t line;

    ok = Exti_GpioToExtiLine(gpio->Peripheral_Ptr->GpioPin, &line);

    if (DEF_TRUE == ok) {
        /* Ensure that the interrupt was registered in the first place. */
        if (NULL == Exti_Callbacks[line]) {
            ok = DEF_FALSE;
        }
    }

    if (DEF_TRUE == ok) {
        Exti_Callbacks[line] = NULL;
        Exti_Args[line] = NULL;
        LL_EXTI_DisableIT_0_31(line);
    }

    return ok;
}


/******************************************
 * ISR callbacks
 ******************************************/
/**
 * @brief  Processes and IRQ for the specified line.
 *
 * @param  line EXTI line to process.
 */
void Exti_GenericIrqHandler(uint32_t line) {
    if (NULL != Exti_Callbacks[line]) {
        if (LL_EXTI_IsActiveFlag_0_31(line) != RESET) {
            LL_EXTI_ClearFlag_0_31(line);
            Exti_Callbacks[line](Exti_Args[line]);
        }
    }
}

/**
  * @brief This function handles EXTI line 0 interrupts.
  */
void EXTI0_IRQHandler(void) {
    Exti_GenericIrqHandler(0U);
}

/**
  * @brief This function handles EXTI line 1 interrupts.
  */
void EXTI1_IRQHandler(void) {
    Exti_GenericIrqHandler(1U);
}

/**
  * @brief This function handles EXTI line 2 interrupts.
  */
void EXTI2_IRQHandler(void) {
    Exti_GenericIrqHandler(2U);
}


/**
  * @brief This function handles EXTI line 3 interrupts.
  */
void EXTI3_IRQHandler(void) {
    Exti_GenericIrqHandler(3U);
}


/**
  * @brief This function handles EXTI line 4 interrupts.
  */
void EXTI4_IRQHandler(void) {
    Exti_GenericIrqHandler(4U);
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI9_5_IRQHandler(void) {
    for (uint32_t i = 5U; i < 9U; i++) {
        Exti_GenericIrqHandler(i);
    }
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void) {
    for (uint32_t i = 10U; i < 15U; i++) {
        Exti_GenericIrqHandler(i);
    }
}


/** @} (end addtogroup EXTI)    */
/** @} (end addtogroup Ports)   */
