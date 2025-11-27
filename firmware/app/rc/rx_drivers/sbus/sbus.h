/**
 * @file     sbus.h
 * @brief    SBUS driver.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __SBUS_H__
#define __SBUS_H__

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "rx_interface.h"

/********************************************************************************
 * Defines
 ********************************************************************************/
#define SBUS_NUM_SERVO_CHANNELS  (16U)
#define SBUS_NUM_SWITCH_CHANNELS (2U)
#define SBUS_NUM_CHANNELS        (SBUS_NUM_SERVO_CHANNELS + SBUS_NUM_SWITCH_CHANNELS)
#define SBUS_FRAME_SIZE_BYTES    (25U)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Interface
 ********************************************************************************/
extern const RXINT_INTERFACE_T Sbus_Interface;


#endif /* __SBUS_H__    */
