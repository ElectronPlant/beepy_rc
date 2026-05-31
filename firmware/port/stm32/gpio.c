/**
 * @file  todo.c
 * @brief TODO
 *
 * @ingroup   GPIO
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

#include "gpio_port.h"
#include "target.h"

#include "stm32f4xx_ll_gpio.h"

#include "gpio.h"


/** @addtogroup Ports
 *    @{
 */

/** @addtogroup GPIO
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Translates the pull from GPIO_PULL_T to stm32 LL driver defines.
 *
 * @param  pull GPIO_PULL_T to translate.
 * @param  p_ll_pull Pointer where the translated pull mode will be stored.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
uint32_t Gpio_TranslatePull(GPIO_PULL_T pull, uint32_t* p_ll_pull) {
    bool_t ok = DEF_TRUE;
    switch (pull) {
        case GPIO_PULL_NONE:
            *p_ll_pull = LL_GPIO_PULL_NO;
            break;

        case GPIO_PULL_DOWN:
            *p_ll_pull = LL_GPIO_PULL_DOWN;
            break;

        case GPIO_PULL_UP:
            *p_ll_pull = LL_GPIO_PULL_UP;
            break;
        default:
            ok = DEF_FALSE;
            break;
    }

    return ok;
}

/**
 * @brief  GPIO
 *
 * @param  gpio Handler for the GPIO to be initialized.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Gpio_Init(GPIO_HANDLER_T gpio) {
    gpio->Peripheral_Ptr->GpioClkEnFn_Ptr(gpio->Peripheral_Ptr->GpioClk);

    switch (gpio->Mode) {
        case GPIO_MODE_INPUT:
            LL_GPIO_SetPinMode(
                gpio->Peripheral_Ptr->GpioPort,
                gpio->Peripheral_Ptr->GpioPin,
                LL_GPIO_MODE_INPUT
            );
            break;
        case GPIO_MODE_OUTPUT:
            LL_GPIO_SetPinMode(
                gpio->Peripheral_Ptr->GpioPort,
                gpio->Peripheral_Ptr->GpioPin,
                LL_GPIO_MODE_OUTPUT
            );
            LL_GPIO_ResetOutputPin(gpio->Peripheral_Ptr->GpioPort, gpio->Peripheral_Ptr->GpioPin);
            break;
        default:
            return DEF_FALSE;
    }

    uint32_t ll_pull;
    if (DEF_FALSE == Gpio_TranslatePull(gpio->Pull, &ll_pull)) {
        return DEF_FALSE;
    }
    LL_GPIO_SetPinPull(gpio->Peripheral_Ptr->GpioPort, gpio->Peripheral_Ptr->GpioPin, ll_pull);

    LL_GPIO_InitTypeDef gpio_init_struct = {
        .Pin = gpio->Peripheral_Ptr->GpioPin,
        .Mode = gpio->Mode == GPIO_MODE_INPUT ? LL_GPIO_MODE_INPUT : LL_GPIO_MODE_OUTPUT,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,       // TODO only low freq is supported.
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL, // TODO only push pull is supported.
        .Pull = ll_pull,
        .Alternate = LL_GPIO_AF_0,
    };

    ErrorStatus init_ok = LL_GPIO_Init(gpio->Peripheral_Ptr->GpioPort, &gpio_init_struct);
    return PLT_UTILS_STM_ERR_STATUS_TO_PLT(init_ok);
}

void Gpio_Write(GPIO_HANDLER_T gpio, GPIO_VALUE_T value) {
    PLT_ASSERT(GPIO_STATUS_INITIALIZED == gpio->Status);
    switch (value) {
        case GPIO_VALUE_LOW:
            LL_GPIO_ResetOutputPin(gpio->Peripheral_Ptr->GpioPort, gpio->Peripheral_Ptr->GpioPin);
            break;
        case GPIO_VALUE_HIGH:
            LL_GPIO_SetOutputPin(gpio->Peripheral_Ptr->GpioPort, gpio->Peripheral_Ptr->GpioPin);
            break;
        default:
            PLT_UNREACHABLE;
    }
}

void Gpio_Toggle(GPIO_HANDLER_T gpio) {
    PLT_ASSERT(GPIO_STATUS_INITIALIZED == gpio->Status);
    LL_GPIO_TogglePin(gpio->Peripheral_Ptr->GpioPort, gpio->Peripheral_Ptr->GpioPin);
}

GPIO_VALUE_T Gpio_Read(GPIO_HANDLER_T gpio) {
    uint32_t reading =
        LL_GPIO_IsInputPinSet(gpio->Peripheral_Ptr->GpioPort, gpio->Peripheral_Ptr->GpioPin);
    return reading == 0x00000000U ? GPIO_VALUE_LOW : GPIO_VALUE_HIGH;
}

/** @} (end addtogroup GPIO)    */
/** @} (end addtogroup Ports)   */
