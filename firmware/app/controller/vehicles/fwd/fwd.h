/**
 * @file  fwd.h
 * @brief Controller for the fwd (four wheel drive) vehicle.
 *
 * @ingroup   FWD
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __FWD_H__
#define __FWD_H__

#include "plt_types.h"

#include "model.h"


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
bool_t Fwd_Init(void);
void   Fwd_Start(void);
void   Fwd_Stop(void);
void   Fwd_RunControlLoop(MODEL_RC_SETPOINT_T* p_frame);
void   Fwd_Disarm(void);

#endif /* __FWD_H__       */
