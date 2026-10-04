/**
 * @file  ui.h
 * @brief User Interface, buttons and status LEDs.
 *
 * @ingroup   Ui
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __UI_H__
#define __UI_H__

#include "plt_types.h"


/** @addtogroup App
 *    @{
 */

/** @addtogroup Supervisor
 *    @{
 */

/** @addtogroup Ui
 *    @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t Ui_Init(void);
void   Ui_Start(void);
void   Ui_RunButtonActions(uint32_t notifications);
void   Ui_SetStatusLeds(uint32_t map, bool_t run_blink);
void   Ui_PowerOff(void);


/** @} (end addtogroup Ui)          */
/** @} (end addtogroup Supervisor)  */
/** @} (end addtogroup App)         */

#endif /* __UI_H__       */
