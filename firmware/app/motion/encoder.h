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

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t          Enc_Init(void);
void            Enc_StartEncoder(void);
void            Enc_ResetCount(void);
uint32_t        Enc_GetCount(void);
ENC_DIRECTION_T Enc_GetDirection(void);

#endif /* __ENCODER_H__       */
