/**
 * @file  control_inputs.h
 * @brief Process RC subscriptions.
 *
 * @ingroup   RcInputs
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __CONTROL_INPUTS_H__
#define __CONTROL_INPUTS_H__

#include "plt_types.h"

#include "model.h"
#include "std_frame.h"

/********************************************************************************
 * Defines
 ********************************************************************************/
#define RCSUBS_INVALID_RC_CHANNEL (255) /* Sentinel channel value to indicate no channel. */


/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
/* Controller task */
bool_t CIn_Init(void);
void   CIn_Start(void);
void   CIn_RunAux(MODEL_RC_SETPOINT_T* p_rc_setpoint);

/* RC task */
void CIn_HandleRcFrame(STD_FRAME_T* p_rc_frame);


#endif /* __CONTROL_INPUTS_H__   */
