/**
 * @file  button.c
 * @brief Button lib.
 *        Generates button interrupts with debouncing.
 *        Warning: The button timing will reset every (2^32 - 1) / configTICK_RATE_HZ seconds.
 *                 with configTICK_RATE_HZ = 1000 it will reset every ~49 days. Pass this point a
 *                 push may be lost.
 *
 * @ingroup   Button
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Button_
 */

#include <stdatomic.h>
#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "FreeRTOS.h"
#include "task.h"

#include "target.h"

#include "button.h"


/** @addtogroup Libs
 *    @{
 */

/** @addtogroup Button
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define BUTTON_DEBOUNCE_TIME_MS (100U)
#define BUTTON_DEBOUNCE_TICKS \
    ((BUTTON_DEBOUNCE_TIME_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)


/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Button_Callback(void* p_data);


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Initializes a button.
 *
 * @param  button Button handler.
 * @param  callback_func Pointer to the button callback function.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Button_Init(BUTTON_HANDLER_T button) {
    bool_t ok = DEF_TRUE;

    if (BUTTON_STATUS_UNINITIALIZED != button->Status) {
        ok = DEF_FALSE;
    }

    if (DEF_TRUE == ok) {
        ok = Gpio_Init(button->Gpio);
    }

    if (DEF_TRUE == ok) {
        button->Status = BUTTON_STATUS_STOPPED;
    }

    return ok;
}

/**
 * @brief  Starts the button interrupts.
 *
 * @param  button Button handler.
 * @param  callback_func Pointer to the button callback function.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Button_Start(BUTTON_HANDLER_T button, BUTTON_CALLBACK_FUNC callback_func) {
    bool_t ok = DEF_TRUE;

    if (BUTTON_STATUS_STOPPED != button->Status || NULL == callback_func) {
        ok = DEF_FALSE;
    }

    if (DEF_TRUE == ok) {
        button->CallbackFunc_Ptr = callback_func;
        button->PrevActivation = (uint32_t)xTaskGetTickCount();

        EXTI_MODE_T mode = BUTTON_PULL_DOWN == button->Pull ? EXTI_MODE_RISING : EXTI_MODE_FALLING;
        ok = Exti_Enable(button->Gpio, Button_Callback, mode, (void*)button);
    }

    if (DEF_TRUE == ok) {
        button->Status = BUTTON_STATUS_RUNNING;
    }

    return ok;
}

/**
 * @brief  Stops the button interrupts.
 *
 * @param  button Button handler.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Button_Stop(BUTTON_HANDLER_T button) {
    bool_t ok = DEF_TRUE;

    if (BUTTON_STATUS_RUNNING != button->Status) {
        ok = DEF_FALSE;
    }

    if (DEF_TRUE == ok) {
        button->CallbackFunc_Ptr = NULL;
        ok = Exti_Disable(button->Gpio);
    }

    if (DEF_TRUE == ok) {
        button->Status = BUTTON_STATUS_STOPPED;
    }

    return ok;
}

/**
 * @brief  Resets the internal timer to prevent it from overflowing.
 *         This is optional if the button is expected to be inactive for ~49 days.
 *
 * @param  button Button handler.
 */
void Button_ResetTimer(BUTTON_HANDLER_T button) {
    if (BUTTON_STATUS_RUNNING == button->Status) {
        uint32_t current_ticks = (uint32_t)xTaskGetTickCount();
        uint32_t new_ticks = current_ticks - BUTTON_DEBOUNCE_TICKS;
        taskENTER_CRITICAL();
        button->PrevActivation = new_ticks;
        taskEXIT_CRITICAL();
    }
}

static void Button_Callback(void* p_data) {
    BUTTON_T* p_button = (BUTTON_T*)p_data;

    uint32_t current_time = (uint32_t)xTaskGetTickCountFromISR();
    if (BUTTON_DEBOUNCE_TICKS < (current_time - p_button->PrevActivation)) {
        if (NULL != p_button->CallbackFunc_Ptr) {
            p_button->CallbackFunc_Ptr();
            p_button->PrevActivation = current_time;
        }
    }
}

/** @} (end addtogroup Button)  */
/** @} (end addtogroup Libs)    */
