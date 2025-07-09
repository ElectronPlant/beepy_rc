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


/********************************************************************************
 * Defines
 ********************************************************************************/
#define SBUS_NUM_SERVO_CHANNELS  (16U)
#define SBUS_NUM_SWITCH_CHANNELS (2U)
#define SBUS_FRAME_SIZE_BYTES    (25U)


/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef struct {
    uint16_t Chn1 : 11;
    uint16_t Chn2 : 11;
    uint16_t Chn3 : 11;
    uint16_t Chn4 : 11;
    uint16_t Chn5 : 11;
    uint16_t Chn6 : 11;
    uint16_t Chn7 : 11;
    uint16_t Chn8 : 11;
    uint16_t Chn9 : 11;
    uint16_t Chn10 : 11;
    uint16_t Chn11 : 11;
    uint16_t Chn12 : 11;
    uint16_t Chn13 : 11;
    uint16_t Chn14 : 11;
    uint16_t Chn15 : 11;
    uint16_t Chn16 : 11;
} PLT_UTILS_PACKED SBUS_FRAME_SERVO_CHANNELS_T;

typedef struct {
    bool_t  Chn17 : 1;
    bool_t  Chn18 : 1;
    bool_t  FrameLost : 1;
    bool_t  FailsafeEnabled : 1;
    uint8_t Unused : 4;
} PLT_UTILS_PACKED SBUS_FRAME_FLAGS_T;

typedef struct {
    uint8_t                     Header;
    SBUS_FRAME_SERVO_CHANNELS_T ServoChn;
    SBUS_FRAME_FLAGS_T          Flags;
    uint8_t                     Footer;
} PLT_UTILS_PACKED SBUS_FRAME_STRUCT_T;

typedef union SBUS_FRAME_U {
    SBUS_FRAME_STRUCT_T Frame;
    uint8_t             Buffer[SBUS_FRAME_SIZE_BYTES];
} SBUS_FRAME_T;


typedef enum {
    SBUS_STATE_IDLE = 0,
    SBUS_STATE_RX_PENDING,
    SBUS_STATE_OK,
    SBUS_STATE_FAILSAFE,
    SBUS_STATE_TIMEOUT,
    SBUS_STATE_INVALID_FRAME,
    SBUS_STATE_FRAME_LOST,
} SBUS_STATE_T;

typedef struct {
    SBUS_STATE_T  State;
    SBUS_FRAME_T *FramePtr; /* At the moment this is the raw frame, it will be parsed first. */
    uint32_t      ReceptionTimeMs; /* Time in ms from when the frame header was received. */
} SBUS_STATUS_T;

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * API
 ********************************************************************************/
bool_t Sbus_Init(void);
bool_t Sbus_StartRx(void);
void   Sbus_TestSizes(void);
void   Sbus_GetFrame(SBUS_STATUS_T *p_data);
void   Sbus_DebugFrame(SBUS_FRAME_T *p_frame);

#endif /* __SBUS_H__    */
