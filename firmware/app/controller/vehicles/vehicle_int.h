/**
 * @file  vehicle_int.h
 * @brief Definition of the generic vehicle interface.
 *
 * @ingroup   VehicleInt
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __VEHICLE_INT_H__
#define __VEHICLE_INT_H__

#include "plt_types.h"


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
/******************************************
 * Interface
 ******************************************/
/**
 * @brief  Interface for vehicle controllers.
 *         This interface provides the abstraction layer for the different vehicles.
 */
typedef struct {
    /**
     * @brief   Initialization function.
     *          Initializes the motors and other elements of the vehicle.
     *
     * @return DEF_TRUE if successful, DEF_FALSE otherwise.
     */
    bool_t (*VInt_Init)(void);

    /**
     * @brief  Starts the vehicle control.
     */
    void (*VInt_Start)(void);

    /**
     * @brief  Stops the vehicle control.
     */
    void (*VInt_Stop)(void);

    /**
     * @brief  Runs the control loop for the vehicle.
     *
     * @param  p_frame Pointer to the RC setpoint.
     */
    void (*VInt_RunControlLoop)(MODEL_RC_SETPOINT_T* p_frame);

    /**
     * @brief  Disarms the vehicle control.
     *         Stops all motors so it is safe to handle.
     */
    void (*VInt_Disarm)(void);
} VINT_INTERFACE_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/


#endif /* __VEHICLE_INT_H__       */
