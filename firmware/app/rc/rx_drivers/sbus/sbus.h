/**
 * @file     sbus.h
 * @brief    SBUS driver.
 *
 * @ingroup   Sbus
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __SBUS_H__
#define __SBUS_H__

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"


/** @addtogroup App
 *   @{
 */

/** @addtogroup Rc
 *   @{
 */

/** @addtogroup RxDriver
 *   @{
 */

/** @addtogroup Sbus
 *   @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
#define SBUS_NUM_SERVO_CHANNELS  (16U)
#define SBUS_NUM_SWITCH_CHANNELS (2U)
#define SBUS_NUM_CHANNELS        (SBUS_NUM_SERVO_CHANNELS)
#define SBUS_FRAME_SIZE_BYTES    (25U)

#define SBUS_SERIAL_BAUDRATE (100000U) // Real: (100000U) Fake: (115200)


/** @} (end addtogroup Sbus)        */
/** @} (end addtogroup RxDriver)    */
/** @} (end addtogroup Rc)          */
/** @} (end addtogroup App)         */

#endif /* __SBUS_H__    */
