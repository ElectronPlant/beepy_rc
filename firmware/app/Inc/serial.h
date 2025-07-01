/**
 * @file     serial.h
 * @brief    Generic Serial implementation.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "plt_types.h"

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
 * API
 ********************************************************************************/
bool_t Serial_Init(void (*int_handler_fn)(void));
bool_t Serial_StartReception(uint8_t *p_data, uint16_t size);
void   Serial_StopReception(void);
void   Serial_IrqHandler(void);


#endif /* __SERIAL_H__       */
