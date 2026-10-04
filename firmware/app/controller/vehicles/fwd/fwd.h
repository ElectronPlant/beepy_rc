/**
 * @file  fwd.h
 * @brief Controller for the fwd (four wheel drive) vehicle.
 *
 * @ingroup   Fwd
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __FWD_H__
#define __FWD_H__

#include "plt_types.h"

#include "model.h"


/** @addtogroup App
 *    @{
 */

/** @addtogroup Controller
 *    @{
 */

/** @addtogroup Vehicles
 *    @{
 */

/** @addtogroup Fwd
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
bool_t Fwd_Init(void);
void   Fwd_Start(void);
void   Fwd_Stop(void);
void   Fwd_RunControlLoop(MODEL_RC_SETPOINT_T* p_frame);
void   Fwd_Disarm(void);


/** @} (end addtogroup Fwd)         */
/** @} (end addtogroup Vehicles)    */
/** @} (end addtogroup Controller)  */
/** @} (end addtogroup App)         */

#endif /* __FWD_H__       */
