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
     *          Configures the peripherals required by the Rx protocol, initializes the driver
     *          handlers, and allocates the Rx buffers.
     *
     * @param  h_bus Rx bus handler.
     * @param  rx_handler_func Pointer to the Rx callback function, it will be called from an ISR.
     * @param  rx_error_func   Pointer to the Rx error callback function, it will be called from an
     *                         ISR.
     *
     * @return DEF_TRUE if successful, DEF_FALSE otherwise.
     */
    bool_t (*VInt_Init)(void);

    /**
     * @brief  Starts the Rx interface.
     *         From this point on the Rx driver should operate autonomously (e.g. using interrupts,
     *         DMA or pulling with SW timers). Once a complete frame is received the driver should
     *         call the callback function to notify the a new frame has been captured.
     *
     * @param  h_bus Rx bus handler.
     *
     * @return DEF_TRUE if successful, DEF_FALSE otherwise.
     */
    void (*VInt_Start)(void);

    /**
     * @brief  Stops the Rx process.
     *
     * @param  h_bus Rx bus handler.
     */
    void (*VInt_Stop)(void);

    /**
     * @brief  Translate the raw received frame into the standard frame representation.
     *
     * @param  p_buffer_info Pointer to the Rx buffer info struct.
     */
    void (*VInt_RunControlLoop)(MODEL_RC_SETPOINT_T* p_frame);

    /**
     * @brief  
     *
     * @param  inp 
     *
     * @return DEF_TRUE if successful, DEF_FALSE otherwise.
     *
     * @note List of notes:
     *       1. 
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
