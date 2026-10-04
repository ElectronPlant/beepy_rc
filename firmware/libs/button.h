/**
 * @file  button.h
 * @brief Button lib
 *
 * @ingroup   Button
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "plt_types.h"

#include "exti.h"
#include "gpio.h"


/** @addtogroup Libs
 *    @{
 */

/** @addtogroup Button
 *    @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum BUTTON_PULL_E {
    BUTTON_PULL_DOWN = 0, /* Active high button */
    BUTTON_PULL_HIGH,     /* Active low button  */
} BUTTON_PULL_T;

typedef void (*BUTTON_CALLBACK_FUNC)(void);

typedef enum BUTTON_STATUS_E {
    BUTTON_STATUS_UNINITIALIZED = 0,
    BUTTON_STATUS_STOPPED,
    BUTTON_STATUS_RUNNING,
} BUTTON_STATUS_T;

typedef struct BUTTON_S {
    GPIO_HANDLER_T       Gpio;
    BUTTON_PULL_T        Pull;
    BUTTON_CALLBACK_FUNC CallbackFunc_Ptr;
    BUTTON_STATUS_T      Status;
    volatile uint32_t    PrevActivation;
} BUTTON_T;

typedef BUTTON_T* BUTTON_HANDLER_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t Button_Init(BUTTON_HANDLER_T button);
bool_t Button_Start(BUTTON_HANDLER_T button, BUTTON_CALLBACK_FUNC callback_func);
bool_t Button_Stop(BUTTON_HANDLER_T button);
void   Button_ResetTimer(BUTTON_HANDLER_T button);
bool_t Button_Confirm(BUTTON_HANDLER_T button);


/** @} (end addtogroup Button)  */
/** @} (end addtogroup Libs)    */

#endif /* __BUTTON_H__       */
