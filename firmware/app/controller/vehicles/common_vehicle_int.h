/**
 * @file  common_vehicle_int.h
 * @brief Abstraction layer for the vehicle controller interface.
 *
 * @ingroup   Vehicles
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __COMMON_VEHICLE_INT_H__
#define __COMMON_VEHICLE_INT_H__

#include "plt_types.h"

#include "model.h"
#include "target.h"
#include "vehicle_int.h"


/********************************************************************************
 * Driver definitions
 ********************************************************************************/
#if TARGET_VEHICLE_TYPE == FWD

bool_t Fwd_Init(void);
void   Fwd_Start(void);
void   Fwd_Stop(void);
void   Fwd_RunControlLoop(MODEL_RC_SETPOINT_T* p_frame);
void   Fwd_Disarm(void);

const VINT_INTERFACE_T CVInt_Interface = {
    .VInt_Init = Fwd_Init,
    .VInt_Start = Fwd_Start,
    .VInt_Stop = Fwd_Stop,
    .VInt_RunControlLoop = Fwd_RunControlLoop,
    .VInt_Disarm = Fwd_Disarm,
};

#else
    #error "Invalid vehicle, review the TARGET_VEHICLE_TYPE definition"
#endif


/********************************************************************************
 * Driver checks
 ********************************************************************************/
/**
 * @brief  Ensures that all interfaces are correctly set.
 *         The check is done through static asserts, so it would not affect the resulting binary.
 */
static inline void ComVInt_CheckInterface(void) {
    PLT_BUILD_ASSERT(NULL != CVInt_Interface.VInt_Init);
    PLT_BUILD_ASSERT(NULL != CVInt_Interface.VInt_Start);
    PLT_BUILD_ASSERT(NULL != CVInt_Interface.VInt_Stop);
    PLT_BUILD_ASSERT(NULL != CVInt_Interface.VInt_RunControlLoop);
    PLT_BUILD_ASSERT(NULL != CVInt_Interface.VInt_Disarm);
}


#endif /* __COMMON_VEHICLE_INT_H__       */
