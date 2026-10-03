/**
 * @file  controller.h
 * @brief Controller - Translates RC channel inputs to actions.
 *
 * @ingroup   Controller
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

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
bool_t Ctrlr_Init(void);
void   Ctrlr_HandleButtonDisarm(void);

#endif /* __CONTROLLER_H__  */
