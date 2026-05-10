/**
 * @file  std_frame.h
 * @brief Standard Rx frame definition.
 *        The standard frame is an intermediate representation of the Rx frame, Designed
 *        to abstract from the Rx protocol and radio used.
 *
 * @ingroup   Rx
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __STD_FRAME_H__
#define __STD_FRAME_H__

#include "plt_types.h"

#include "common_rx_sizes.h"

/********************************************************************************
 * Defines
 ********************************************************************************/
#define STD_FRAME_NUM_CHANNELS (COMRXS_NUM_CHANNELS)
#define STD_FRAME_LOW_LIMIT    (-100.0f)
#define STD_FRAME_HIGH_LIMIT   (100.0f)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/**
 * @brief Standard Frame States.
 *        Common states for the SBUS frames.
 */
typedef enum {
    STD_FRAME_STATE_UNDEFINED = 0, /**< Default value for when the state is pending to be set */
    STD_FRAME_STATE_INVALID,       /**< Received frame is incorrect */
    STD_FRAME_STATE_VALID,         /**< The frame is valid */
    STD_FRAME_STATE_FAILSAFE,      /**< Failsafe has been activated */
    STD_FRAME_STATE_DROPPED,       /**< The frame could not be processed in time */
    STD_FRAME_STATE_TIMEOUT,       /**< Too much time since the last frame. */
} STD_FRAME_STATE_T;

/**
 * @brief Standard Frame
 *        The standard frame is composed by channels for each input.
 *        Channels are represented by a float value between [-100, 100].
 */
typedef struct {
    float32_t         Channels[STD_FRAME_NUM_CHANNELS];
    STD_FRAME_STATE_T State;
    uint32_t          RxTime;
} STD_FRAME_T;

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/


#endif /* __STD_FRAME_H__       */
