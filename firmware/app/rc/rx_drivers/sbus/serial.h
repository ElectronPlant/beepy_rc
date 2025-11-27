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
bool_t Serial_Init(void (*rx_handler_func)(uint8_t), void (*error_handler_func)(void));
bool_t Serial_StartReception(void);
void   Serial_StopReception(void);


#endif /* __SERIAL_H__       */
