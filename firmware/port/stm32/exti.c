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

#include "priorities_cfg.h"

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
#define EXTI_MAX_INTERRUPT_LINES (16U)


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

bool_t Exti_GpioToExtiSysCfgLine(uint32_t gpio, uint32_t* p_exti) {
    bool_t ok = DEF_TRUE;

    switch (gpio) {
        case LL_GPIO_PIN_0:
            *p_exti = LL_SYSCFG_EXTI_LINE0;
            break;
        case LL_GPIO_PIN_1:
            *p_exti = LL_SYSCFG_EXTI_LINE1;
            break;
        case LL_GPIO_PIN_2:
            *p_exti = LL_SYSCFG_EXTI_LINE2;
            break;
        case LL_GPIO_PIN_3:
            *p_exti = LL_SYSCFG_EXTI_LINE3;
            break;
        case LL_GPIO_PIN_4:
            *p_exti = LL_SYSCFG_EXTI_LINE4;
            break;
        case LL_GPIO_PIN_5:
            *p_exti = LL_SYSCFG_EXTI_LINE5;
            break;
        case LL_GPIO_PIN_6:
            *p_exti = LL_SYSCFG_EXTI_LINE6;
            break;
        case LL_GPIO_PIN_7:
            *p_exti = LL_SYSCFG_EXTI_LINE7;
            break;
        case LL_GPIO_PIN_8:
            *p_exti = LL_SYSCFG_EXTI_LINE8;
            break;
        case LL_GPIO_PIN_9:
            *p_exti = LL_SYSCFG_EXTI_LINE9;
            break;
        case LL_GPIO_PIN_10:
            *p_exti = LL_SYSCFG_EXTI_LINE10;
            break;
        case LL_GPIO_PIN_11:
            *p_exti = LL_SYSCFG_EXTI_LINE11;
            break;
        case LL_GPIO_PIN_12:
            *p_exti = LL_SYSCFG_EXTI_LINE12;
            break;
        case LL_GPIO_PIN_13:
            *p_exti = LL_SYSCFG_EXTI_LINE13;
            break;
        case LL_GPIO_PIN_14:
            *p_exti = LL_SYSCFG_EXTI_LINE14;
            break;
        case LL_GPIO_PIN_15:
            *p_exti = LL_SYSCFG_EXTI_LINE15;
            break;
        default:
            ok = DEF_FALSE;
            break;
    }
    return ok;
}

bool_t Exti_ExtiLineToIndex(uint32_t exti_line, uint16_t* p_index) {
    bool_t ok = DEF_TRUE;

    switch (exti_line) {
        case LL_EXTI_LINE_0:
            *p_index = 0;
            break;
        case LL_EXTI_LINE_1:
            *p_index = 1;
            break;
        case LL_EXTI_LINE_2:
            *p_index = 2;
            break;
        case LL_EXTI_LINE_3:
            *p_index = 3;
            break;
        case LL_EXTI_LINE_4:
            *p_index = 4;
            break;
        case LL_EXTI_LINE_5:
            *p_index = 5;
            break;
        case LL_EXTI_LINE_6:
            *p_index = 6;
            break;
        case LL_EXTI_LINE_7:
            *p_index = 7;
            break;
        case LL_EXTI_LINE_8:
            *p_index = 8;
            break;
        case LL_EXTI_LINE_9:
            *p_index = 9;
            break;
        case LL_EXTI_LINE_10:
            *p_index = 10;
            break;
        case LL_EXTI_LINE_11:
            *p_index = 11;
            break;
        case LL_EXTI_LINE_12:
            *p_index = 12;
            break;
        case LL_EXTI_LINE_13:
            *p_index = 13;
            break;
        case LL_EXTI_LINE_14:
            *p_index = 14;
            break;
        case LL_EXTI_LINE_15:
            *p_index = 15;
            break;
        default:
            ok = DEF_FALSE;
            break;
    }
    return ok;
}

static inline uint32_t Exti_ExtiIndexToLine(uint16_t index) {
    return 0x1UL << index;
}

static bool_t Exti_PortToExtiPort(GPIO_HANDLER_T gpio, uint32_t* p_port) {
    bool_t ok = DEF_TRUE;

    switch ((uint32_t)gpio->Peripheral_Ptr->GpioPort) {
        case (uint32_t)GPIOA:
            *p_port = LL_SYSCFG_EXTI_PORTA;
            break;
        case (uint32_t)GPIOB:
            *p_port = LL_SYSCFG_EXTI_PORTB;
            break;
        case (uint32_t)GPIOC:
            *p_port = LL_SYSCFG_EXTI_PORTC;
            break;
        case (uint32_t)GPIOD:
            *p_port = LL_SYSCFG_EXTI_PORTD;
            break;
        case (uint32_t)GPIOE:
            *p_port = LL_SYSCFG_EXTI_PORTE;
            break;
        case (uint32_t)GPIOF:
            *p_port = LL_SYSCFG_EXTI_PORTF;
            break;
        case (uint32_t)GPIOG:
            *p_port = LL_SYSCFG_EXTI_PORTG;
            break;
        case (uint32_t)GPIOH:
            *p_port = LL_SYSCFG_EXTI_PORTH;
            break;
        default:
            ok = DEF_FALSE;
    }
    return ok;
}

static bool_t Exti_GpioToIrqNumber(uint32_t pin, uint32_t* p_irq_number) {
    bool_t ok = DEF_TRUE;
    switch (pin) {
        case LL_GPIO_PIN_0:
            *p_irq_number = EXTI0_IRQn;
            break;
        case LL_GPIO_PIN_1:
            *p_irq_number = EXTI1_IRQn;
            break;
        case LL_GPIO_PIN_2:
            *p_irq_number = EXTI2_IRQn;
            break;
        case LL_GPIO_PIN_3:
            *p_irq_number = EXTI3_IRQn;
            break;
        case LL_GPIO_PIN_4:
            *p_irq_number = EXTI4_IRQn;
            break;
        case LL_GPIO_PIN_5:
        case LL_GPIO_PIN_6:
        case LL_GPIO_PIN_7:
        case LL_GPIO_PIN_8:
        case LL_GPIO_PIN_9:
            *p_irq_number = EXTI9_5_IRQn;
            break;
        case LL_GPIO_PIN_10:
        case LL_GPIO_PIN_11:
        case LL_GPIO_PIN_12:
        case LL_GPIO_PIN_13:
        case LL_GPIO_PIN_14:
        case LL_GPIO_PIN_15:
            *p_irq_number = EXTI15_10_IRQn;
            break;
        default:
            ok = DEF_FALSE;
            break;
    }
    return ok;
}

bool_t Exti_EnableMode(EXTI_MODE_T mode, uint32_t line) {
    bool_t ok = DEF_TRUE;

    /* First Disable Event on provided Lines */
    LL_EXTI_DisableEvent_0_31(line);
    /* Then Enable IT on provided Lines */
    LL_EXTI_EnableIT_0_31(line);

    switch (mode) {
        case EXTI_MODE_BOTH:
            LL_EXTI_EnableIT_0_31(line);
            break;
        case EXTI_MODE_RISING:
            LL_EXTI_DisableFallingTrig_0_31(line);
            LL_EXTI_EnableRisingTrig_0_31(line);
            break;
        case EXTI_MODE_FALLING:
            LL_EXTI_DisableRisingTrig_0_31(line);
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
    static bool_t initialized = DEF_FALSE;

    bool_t   ok;
    uint32_t line = 0;
    uint32_t sys_cfg_line = 0;
    uint16_t index = 0;
    uint32_t port = 0;
    uint32_t irq_number = 0;

    ok = Exti_GpioToExtiLine(gpio->Peripheral_Ptr->GpioPin, &line);
    ok &= Exti_GpioToExtiSysCfgLine(gpio->Peripheral_Ptr->GpioPin, &sys_cfg_line);
    ok &= Exti_ExtiLineToIndex(line, &index);
    ok &= Exti_PortToExtiPort(gpio, &port);
    ok &= Exti_GpioToIrqNumber(gpio->Peripheral_Ptr->GpioPin, &irq_number);

    /* Ensure that the GPIO has the correct mode, the new callback is not NULL and that
       there are no prior interrupts registered to that line.
    */
    if (GPIO_MODE_INPUT != gpio->Mode || NULL == callback || NULL != Exti_Callbacks[index]) {
        return DEF_FALSE;
    }

    if (DEF_TRUE == ok) {
        if (DEF_FALSE == initialized) {
            LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
            initialized = DEF_TRUE;
        }

        Exti_Callbacks[index] = callback;
        Exti_Args[index] = p_arg;

        LL_SYSCFG_SetEXTISource(port, sys_cfg_line);
        Exti_EnableMode(mode, line);

        LL_EXTI_ClearFlag_0_31(line);
        (void)LL_EXTI_IsActiveFlag_0_31(line); /* Wait for the flag to be cleared */

        NVIC_SetPriority(
            irq_number,
            NVIC_EncodePriority(NVIC_GetPriorityGrouping(), PRIORITIES_CFG_BUTTON_IRQ_PRIORITY, 0)
        );
        NVIC_EnableIRQ(irq_number);
    }
    return ok;
}

bool_t Exti_Disable(GPIO_HANDLER_T gpio) {
    bool_t   ok = DEF_TRUE;
    uint32_t line;
    uint16_t index;

    ok = Exti_GpioToExtiLine(gpio->Peripheral_Ptr->GpioPin, &line);
    Exti_GpioToExtiLine(gpio->Peripheral_Ptr->GpioPin, &line);
    if (DEF_TRUE == ok) {
        ok = Exti_ExtiLineToIndex(line, &index);
    }

    if (DEF_TRUE == ok) {
        /* Ensure that the interrupt was registered in the first place. */
        if (NULL == Exti_Callbacks[index]) {
            ok = DEF_FALSE;
        }
    }

    if (DEF_TRUE == ok) {
        Exti_Callbacks[index] = NULL;
        Exti_Args[index] = NULL;
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
void Exti_GenericIrqHandler(uint16_t index) {
    uint32_t line = Exti_ExtiIndexToLine(index);
    if (NULL != Exti_Callbacks[index]) {
        if (LL_EXTI_IsActiveFlag_0_31(line) != RESET) {
            LL_EXTI_ClearFlag_0_31(line);
            Exti_Callbacks[index](Exti_Args[index]);
            (void)LL_EXTI_IsActiveFlag_0_31(line);
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
    for (uint32_t i = 5U; i <= 9U; i++) {
        Exti_GenericIrqHandler(i);
    }
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void) {
    for (uint32_t i = 10U; i <= 15U; i++) {
        Exti_GenericIrqHandler(i);
    }
}


/** @} (end addtogroup EXTI)    */
/** @} (end addtogroup Ports)   */
