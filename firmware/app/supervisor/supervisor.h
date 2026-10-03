/**
 * @file  supervisor.h
 * @brief Supervisor task to control User Interface (UI) and internal state.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __SUPERVISOR_H__
#define __SUPERVISOR_H__

#include "plt_types.h"


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
bool_t Super_Init(void);

void Super_NotifyButton1(void);
void Super_NotifyButton2(void);

void Super_NotifyRcRunning(void);
void Super_NotifyRcDisconnected(void);
void Super_NotifyRcReconnected(void);
void Super_NotifyRcError(void);
void Super_NotifyArmed(void);
void Super_NotifyDisarmed(void);
void Super_NotifyControllerError(void);


#endif /* __SUPERVISOR_H__      */
