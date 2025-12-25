/**
 * @file  encoder.h
 * @brief Driver for the motor encoders.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "plt_types.h"


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum ENC_DIRECTION_E {
    ENC_DIRECTION_UP = 0,
    ENC_DIRECTION_DOWN,
} ENC_DIRECTION_T;

typedef enum ENC_STATUS_E {
    ENC_STATUS_UNINITIALIZED = 0,
    ENC_STATUS_INITIALIZED,
    ENC_STATUS_RUNNING,
} ENC_STATUS_T;

typedef const ENC_PERIPHERAL_PORT_T* ENC_PERIPHERAL_T;

typedef struct ENC_INSTANCE_S {
    ENC_STATUS_T           Status; /* Must be set to ENC_STATUS_UNINITIALIZED on the struct def */
    const ENC_PERIPHERAL_T Peripheral;
} ENC_INSTANCE_T;

typedef ENC_INSTANCE_T* ENC_HANDLER_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t          Enc_Init(ENC_HANDLER_T enc);
void            Enc_StartEncoder(ENC_HANDLER_T enc);
void            Enc_StopEncoder(ENC_HANDLER_T enc);
void            Enc_ResetCount(ENC_HANDLER_T enc);
uint32_t        Enc_GetCount(ENC_HANDLER_T enc);
ENC_DIRECTION_T Enc_GetDirection(ENC_HANDLER_T enc);

#endif /* __ENCODER_H__       */
