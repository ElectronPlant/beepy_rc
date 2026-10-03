/**
 * @file  ui.c
 * @brief User Interface, buttons and status LEDs.
 *
 * @ingroup   Ui
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
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
#define UI_BUTTON_ID_POWER_OFF (1U)
#define UI_BUTTON_ID_DISARM    (0U)

/* -- Power off delay -- */
#define SUPER_POWER_OFF_DELAY_MS (250U)
#define SUPER_POWER_OFF_DELAY_TICKS \
    ((SUPER_POWER_OFF_DELAY_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)


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

    /* Status LEDs */
    for (uint8_t led = 0; UI_REQUIRED_LEDS > led && DEF_TRUE == ok; led++) {
        ok = Gpio_Init(Target_Leds[led]);
    }

    /* Buttons */
    for (uint8_t button = 0; UI_REQUIRED_BUTTONS > button && DEF_TRUE == ok; button++) {
        ok = Button_Init(Target_Buttons[button]);
    }

    /* Power Off */
    if (DEF_TRUE == ok) {
        ok = Gpio_Init(Target_BatEnable);
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

/******************************************
 * Buttons
 ******************************************/

/**
 * @brief  Disarm button callback function.
 *         Callback function for button 1. Disarms the vehicle.
 */
void Ui_Button1Callback(void) {
    Super_NotifyButton1();
}

/**
 * @brief  Power off button callback function.
 *         Callback function for button 2. Turns off the board power.
 */
void Ui_Button2Callback(void) {
    Super_NotifyButton2();
}

/**
 * @brief  Disarms the controller.
 *
 * @note List of notes:
 *       1. Once disarmed, it can only be rearmed by toggling the arm switch to set it to the
 *          disarm state, then arming it again.
 */
static void Ui_ButtonDisarm(BUTTON_HANDLER_T button) {
    if (DEF_TRUE == Button_Confirm(button)) {
        printf("\n\n\nButton Disarm\n\n\n");
        Ctrlr_HandleButtonDisarm();
    } else {
        printf("\n---Button disarm ignored\n");
    }
}

/**
 * @brief  Powers off the board.
 *
 * @note List of notes:
 *       1. Once powered off, it can only be turned back on by removing the battery for a few
 *          seconds and connecting it back.
 */
static void Ui_ButtonPowerOff(BUTTON_HANDLER_T button) {
    if (DEF_TRUE == Button_Confirm(button)) {
        printf("\n\n\nButton Power off\n\n\n");
        Ui_PowerOff();
    } else {
        printf("\n---Button power off ignored\n");
    }
}

/**
 * @brief  Runs the button actions
 *
 * @param  notifications Task notifications.
 */
void Ui_RunButtonActions(uint32_t notifications) {
    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_BUTTON_1)) {
        Ui_ButtonDisarm(Target_Buttons[UI_BUTTON_ID_DISARM]);
    }

    if (PLT_UTILS_IS_BIT_OFFSET_SET(notifications, SUPERDEF_SOURCE_OFFSET_BUTTON_2)) {
        printf("Button power off...\n");
        vTaskDelay(SUPER_POWER_OFF_DELAY_TICKS);
        Ui_ButtonPowerOff(Target_Buttons[UI_BUTTON_ID_POWER_OFF]);
    }
}


/******************************************
 * Status LEDs
 ******************************************/

/**
 * @brief  Sets the status LEDs.
 *         This function is ment to be called periodically to keep the Status LEDs up to date and
 *         handle blinking.
 *
 *         LED Meaning:
 *              * RED LED:
 *                  - OFF => No error active.
 *                  - BLINKING => RC connection lost.
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
    /* Yellow LED */
    bool_t rc_aligned = PLT_UTILS_IS_BIT_OFFSET_SET(map, SUPERDEF_CONTEXT_OFFSET_RC_ALIGNED);
    if (DEF_TRUE == rc_aligned) {
        Gpio_Write(Target_Leds[YELLOW], GPIO_VALUE_HIGH);
    } else if (DEF_TRUE == run_blink) {
        Gpio_Toggle(Target_Leds[YELLOW]);
    }

    /* Green LED */
    bool_t armed = PLT_UTILS_IS_BIT_OFFSET_SET(map, SUPERDEF_CONTEXT_OFFSET_ARMED);
    Gpio_Write(Target_Leds[GREEN], GPIO_BOOL_TO_VALUE(armed));

    /* Red LED */
    bool_t error_active = SuperDef_IsAnyErrorSet(map);
    bool_t connection_ok = PLT_UTILS_IS_BIT_OFFSET_SET(map, SUPERDEF_CONTEXT_OFFSET_RC_CONNECTED);
    if (DEF_TRUE == error_active) {
        Gpio_Write(Target_Leds[RED], GPIO_VALUE_HIGH);
    } else if (DEF_TRUE == rc_aligned && DEF_FALSE == connection_ok) {
        if (DEF_TRUE == run_blink) {
            Gpio_Toggle(Target_Leds[RED]);
        }
    } else {
        Gpio_Write(Target_Leds[RED], GPIO_VALUE_LOW);
    }
}

/******************************************
 * Power off.
 ******************************************/

/**
 * @brief  Powers off the board.
 *
 * @note List of notes:
 *       1. Once powered off, it can only be turned back on by removing the battery for a few
 *          seconds and connecting it back.
 */
void Ui_PowerOff(void) {
    printf("Powering off...\n");
    Gpio_Write(Target_BatEnable, GPIO_VALUE_HIGH);
}

/** @} (end addtogroup UI)  */
/** @} (end addtogroup Supervisor)  */
