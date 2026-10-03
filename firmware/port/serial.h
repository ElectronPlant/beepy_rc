/**
 * @file     serial.h
 * @brief    Generic Serial implementation.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
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
typedef enum SERIAL_STATUS_E {
    SERIAL_STATUS_UNINITIALIZED = 0,
    SERIAL_STATUS_INITIALIZED,
    SERIAL_STATUS_RUNNING,
} SERIAL_STATUS_T;

typedef const SERIAL_PERIPHERAL_PORT_T* SERIAL_PERIPHERAL_T;

typedef struct SERIAL_INSTANCE_S {
    SERIAL_STATUS_T Status; /* Must be set to ENC_STATUS_UNINITIALIZED on the struct def */
    const SERIAL_PERIPHERAL_T Peripheral;
    void (*RxHandlerFunct_Ptr)(uint8_t);
    void (*ErrorHandlerFunct_Ptr)(void);
    uint8_t  IrqPriority;
    uint32_t BaudRate;
} SERIAL_INSTANCE_T;

typedef SERIAL_INSTANCE_T* SERIAL_HANDLER_T;

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * API
 ********************************************************************************/
/**
 * @brief  Initializes the serial interface. See note 1.
 *
 * @param  ser: Serial handler.
 * @param  rx_handler_func: Pointer to the function that will be executed everytime a new byte is
 *                          received. Note that it is executed as part of the serial ISR.
 * @param  error_handler_func: Pointer to the function that will be executed when ever there has
 *                             been an Rx error. Note that it is executed as part of the serial ISR.
 *                             When no error external error handling is required it may be set to
 *                             NULL.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. The serial must be uninitialized.
 */
bool_t Serial_Init(
    SERIAL_HANDLER_T ser,
    void (*rx_handler_func)(uint8_t),
    void (*error_handler_func)(void)
);

/**
 * @brief  Starts the serial reception.
 *         Once the serial interface is enabled, it will continuously listen for any incoming byte
 *         to call the interrupt. This will continue until the stop is executed. See note 1.
 *
 * @param  ser: Serial handler.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. The serial must be initialized but not started.
 */
bool_t Serial_StartReception(SERIAL_HANDLER_T ser);

/**
 * @brief  Stops the serial reception. See note 1.
 *
 * @param  ser: Serial handler.
 *
 * @note List of notes:
 *       1. The serial must be running.
 */
void Serial_StopReception(SERIAL_HANDLER_T ser);


/******************************************
 * IRQ
 ******************************************/

/**
 * @brief  Handles the incoming byte.
 *
 * @param ser: Serial peripheral handler.
 *
 * @return Received byte.
 */
uint8_t Serial_HandleRx(SERIAL_HANDLER_T ser);

/**
 * @brief Handles error encountered while receiving data.
 *
 * @param h_serial: Serial peripheral.
 *
 * @note List of notes:
 *      1. The process the handle the errors is just to clear the flag and flush the data.
 */
void Serial_HandleError(SERIAL_HANDLER_T ser);

/**
 * @brief  Checks if the Serial error flag is enabled.
 *
 * @param ser: Serial peripheral handler.
 *
 * @return DEF_TRUE if the error flag is enabled, DEF_FALSE otherwise.
 */
bool_t Serial_IsErrorFlagEnabled(SERIAL_HANDLER_T ser);

/**
 * @brief  Checks if the Serial error flag is enabled.
 *
 * @param ser: Serial peripheral handler.
 *
 * @return DEF_TRUE if the error flag is enabled, DEF_FALSE otherwise.
 */
bool_t Serial_IsIrqEnabled(SERIAL_HANDLER_T ser);

#define SERIAL_IRQ_CALLBACK(FUNC, CALLBACK_ERR, CALLBACK_RX, HANDLER_GETTER) \
    void FUNC(void) { \
        PLT_BUILD_ASSERT(NULL != CALLBACK_ERR); \
        PLT_BUILD_ASSERT(NULL != CALLBACK_RX); \
        PLT_BUILD_ASSERT(NULL != HANDLER_GETTER); \
        SERIAL_HANDLER_T h_serial = (SERIAL_HANDLER_T)HANDLER_GETTER(); \
        if (DEF_TRUE == Serial_IsErrorFlagEnabled(h_serial)) { \
            Serial_HandleError(h_serial); \
            CALLBACK_ERR(); \
        } else if (DEF_TRUE == Serial_IsIrqEnabled(h_serial)) { \
            uint8_t rx_byte = Serial_HandleRx(h_serial); \
            CALLBACK_RX(rx_byte); \
        } \
    }


#endif /* __SERIAL_H__       */
