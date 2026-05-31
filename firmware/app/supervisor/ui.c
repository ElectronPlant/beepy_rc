/**
 * @file  ui.c
 * @brief User Interface, buttons and status LEDs.
 *
 * @ingroup   Ui
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Ui_
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "target.h"

#include "button.h"
#include "exti.h"
#include "gpio.h"

#include "controller.h"
#include "supervisor.h"
#include "supervisor_defines.h"
#include "ui.h"


/** @addtogroup Supervisor
 *    @{
 */

/** @addtogroup UI
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum UI_LED_NAME_E {
    GREEN = 0,
    YELLOW,
    RED,

    UI_REQUIRED_LEDS,
} UI_LED_NAME_T;

typedef enum UI_BUTTON_NAME_E {
    BUTTON_1 = 0,
    BUTTON_2,

    UI_REQUIRED_BUTTONS,
} UI_BUTTON_NAME_T;

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
void Ui_Button1Callback(void);
void Ui_Button2Callback(void);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
const BUTTON_CALLBACK_FUNC ButtonCallbacks[UI_REQUIRED_BUTTONS] = {
    Ui_Button1Callback,
    Ui_Button2Callback,
};


/********************************************************************************
 * Function Implementations
 ********************************************************************************/
/**
 * @brief  Initializes the UI.
 *           - Status LEDs.
 *           - UI buttons.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Ui_Init(void) {
    bool_t ok = DEF_TRUE;

    PLT_BUILD_ASSERT(TARGET_NUM_LEDS >= UI_REQUIRED_LEDS);
    PLT_BUILD_ASSERT(TARGET_NUM_BUTTONS >= UI_REQUIRED_BUTTONS);

    for (uint8_t led = 0; UI_REQUIRED_LEDS > led && DEF_TRUE == ok; led++) {
        ok = Gpio_Init(Target_Leds[led]);
    }

    for (uint8_t button = 0; UI_REQUIRED_BUTTONS > button && DEF_TRUE == ok; button++) {
        ok = Button_Init(Target_Buttons[button]);
    }

    return ok;
}

/**
 * @brief  Starts the UI.
 */
void Ui_Start(void) {
    for (uint8_t button = 0; UI_REQUIRED_BUTTONS > button; button++) {
        bool_t ok = Button_Start(Target_Buttons[button], ButtonCallbacks[button]);
        PLT_ASSERT(DEF_TRUE == ok);
    }
}

/**
 * @brief  Disarm button callback function.
 *         Callback function for button 1. Disarms the vehicle.
 */
void Ui_Button1Callback(void) {
    Ctrlr_HandleButtonDisarm();
}

/**
 * @brief  Power off button callback function.
 *         Callback function for button 2. Turns off the board power.
 */
void Ui_Button2Callback(void) {
    // TODO - Power off
}

/**
 * @brief  Sets the status LEDs.
 *         This function is ment to be called periodically to keep the Status LEDs up to date and
 *         handle blinking.
 *
 *         LED Meaning:
 *              * RED LED:
 *                  - OFF => No error active.
 *                  - ON  => At least one error active.
 *              * YELLOW LED:
 *                  - BLINKING => Waitin for RC alignment.
 *                  - ON => RC alignment.
 *              * GREEN LED:
 *                  - OFF => Disarmed.
 *                  - ON  => Armed.
 *
 * @param  map Context status map.
 * @param  run_blink Flag to control if the LED blink should update.
 */
void Ui_SetStatusLeds(uint32_t map, bool_t run_blink) {
    /* Red LED */
    bool_t error_active = SuperDef_IsAnyErrorSet(map);
    Gpio_Write(Target_Leds[RED], error_active);

    /* Green LED */
    bool_t armed = PLT_UTILS_IS_BIT_OFFSET_SET(map, SUPERDEF_SOURCE_OFFSET_ARMED);
    Gpio_Write(Target_Leds[GREEN], armed);

    /* Yellow LED */
    bool_t rc_aligned = PLT_UTILS_IS_BIT_OFFSET_SET(map, SUPERDEF_CONTEXT_OFFSET_RC_ALIGNEND);
    if (DEF_TRUE == rc_aligned) {
        Gpio_Write(Target_Leds[YELLOW], rc_aligned);
    } else if (DEF_TRUE == run_blink) {
        Gpio_Toggle(Target_Leds[YELLOW]);
    }
}

/** @} (end addtogroup UI)  */
/** @} (end addtogroup Supervisor)  */
