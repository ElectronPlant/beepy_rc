/**
 * @file  encoder.h
 * @brief Driver for the motor encoders.
 *
 * @ingroup   Encoder
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "plt_types.h"


/** @addtogroup Port
 *    @{
 */

/** @addtogroup Encoder
 *    @{
 */

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
/**
 * @brief  Initializes the encoder. See note 1.
 *         Encoders are composed from a timer and two GPIO inputs for the quadrature signal.
 *
 * @param  enc Encoder handler.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. The encoder instance must be uninitialized before being uninitialized.
 */
bool_t Enc_Init(ENC_HANDLER_T enc);

/**
 * @brief  Starts the encoder count. See note 1.
 *
 * @param  enc Encoder handler.
 *
 * @note List of notes:
 *       1. The encoder must be initialized to be started.
 */
void Enc_StartEncoder(ENC_HANDLER_T enc);

/**
 * @brief  Stops the encoder count. See note 1.
 *
 * @param  enc Encoder handler.
 *
 * @note List of notes:
 *       1. The encoder must be running before it can be stopped.
 */
void Enc_StopEncoder(ENC_HANDLER_T enc);

/**
 * @brief  Reads the encoder count. See note 1.
 *
 * @param  enc Encoder handler.
 *
 * @return Current encoder count.
 *
 * @note List of notes:
 *       1. The encoder must be running to perform a correct count reading.
 */
uint32_t Enc_GetCount(ENC_HANDLER_T enc);

/**
 * @brief  Resets the encoder count to zero. See note 1.
 *
 * @param  enc Encoder handler.
 *
 *  @note List of notes:
 *       1. The encoder must be running to reset the count.
 */
void Enc_ResetCount(ENC_HANDLER_T enc);

/**
 * @brief  Reads the count direction of the encoder. See note 1.
 *
 * @param  enc Encoder handler.
 *
 * @return ENC_DIRECTION_UP if the encoder direction is counting up, ENC_DIRECTION_DOWN otherwise.
 *
 * @note List of notes:
 *       1. The encoder must be running to get a valid direction reading.
 */
ENC_DIRECTION_T Enc_GetDirection(ENC_HANDLER_T enc);


/** @} (end addtogroup Port)        */
/** @} (end addtogroup Encoder)     */

#endif /* __ENCODER_H__       */
