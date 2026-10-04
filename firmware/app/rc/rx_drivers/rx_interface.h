/**
 * @file  rx_interface.h
 * @brief Common interface for the Rx protocols.
 *
 * @ingroup   RxInterface
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: RxInt_
 */

#ifndef __RX_INTERFACE_H__
#define __RX_INTERFACE_H__

#include "plt_types.h"

#include "common_rx_bus.h"
#include "std_frame.h"


/** @addtogroup App
 *   @{
 */

/** @addtogroup Rc
 *   @{
 */

/** @addtogroup RxDriver
 *   @{
 */

/** @addtogroup RxInterface
 *   @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
#define RXINT_MAX_PARALLEL_RAW_BUFFERS (5u)


/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum {
    RXINT_DRIVER_STATUS_UNINITIALIZED = 0,
    RXINT_DRIVER_STATUS_STOPPED,
    RXINT_DRIVER_STATUS_MISALIGNED,
    RXINT_DRIVER_STATUS_WAITING_FOR_ALIGNMENT,
    RXINT_DRIVER_STATUS_RUNNING,
    RXINT_DRIVER_STATUS_ERROR,
} RXINT_DRIVER_STATUS_T;

typedef enum {
    RXINT_ERROR_NONE = 0,                 /**< Reserved in case there are no errors */
    RXINT_ERROR_MODULE_INIT,              /**< Failed to initialize the RC module */
    RXINT_ERROR_DRIVER_INIT,              /**< The driver failed to initialize correctly */
    RXINT_ERROR_DRIVER_ALIGNMENT_TIMEOUT, /**< Failed to complete the alignment process (timeout) */
    RXINT_ERROR_DRIVER_RUNTIME,    /**< Driver failed while running (e.g. serial bus error, etc.) */
    RXINT_RX_ERROR_QUEUE_OVERFLOW, /**< Too many requests in the queue */

    RXINT_RX_ERROR_DRIVER_MAX, /**< Invalid error to get the number of defined errors. */
} RXINT_ERRORS_T;

typedef struct {
    uint8_t* RxBufferPtr;
    uint16_t RxBufferSize;
} RXINT_RX_BUFFER_INFO_T;

typedef enum {
    RXINT_ALIGNMENT_STATUS_UNINITIALIZED = 0,  /**< Alignment has not started yet.               */
    RXINT_ALIGNMENT_STATUS_WAITING_FOR_HEADER, /**< Waiting for the header byte to be received.  */
    RXINT_ALIGNMENT_STATUS_WAITING_FOR_FRAME,  /**< Waiting for a full frame to be received.     */
    RXINT_ALIGNMENT_STATUS_COMPLETED,          /**< Alignment has successfully completed.        */
    RXINT_ALIGNMENT_STATUS_FAILED,             /**< Alignment failed and needs to be restarted.  */
} RXINT_ALIGNMENT_STATUS;

typedef struct {
    RXINT_RX_BUFFER_INFO_T RxBufferInfo;
    union {
        uint16_t RxCount;     /**< Number of bytes in the Rx buffer. */
        uint16_t HeaderCount; /**< Number of bytes received while waiting for the header.
                                   (only used during the initialization while in the
                                    RXINT_ALIGNMENT_STATUS_WAITING_FOR_HEADER state) */
    } Count;
    RXINT_ALIGNMENT_STATUS AlignmentStatus;
} RXINT_RX_INFO_T;


/******************************************
 * Callbacks
 ******************************************/
/**
 * @brief  Byte received callback function.
 *         This function is called within the Rx ISR, and is called each time a new byte is
 *         received.
 *
 * @param  rx_byte Read byte.
 */
typedef void (*RxInt_RxHandler)(uint8_t rx_byte);

/**
 * @brief  Rx error callback function.
 *         This function is called within the Rx ISR if an Rx error is detected.
 */
typedef void (*RxInt_RxErrorHandler)(void);

/******************************************
 * Interface
 ******************************************/
/**
 * @brief  Interface for the Rx protocols.
 *         This interface provides the abstraction layer for the different Rx protocols (e.g. SBUS).
 */
typedef struct {
    /**
     * @brief   Initialization function.
     *          Configures the peripherals required by the Rx protocol, initializes the driver
     *          handlers, and allocates the Rx buffers.
     *
     * @param  h_bus Rx bus handler.
     * @param  rx_handler_func Pointer to the Rx callback function, it will be called from an ISR.
     * @param  rx_error_func   Pointer to the Rx error callback function, it will be called from an
     *                         ISR.
     *
     * @return DEF_TRUE if successful, DEF_FALSE otherwise.
     */
    bool_t (*RxInt_Init)(
        COM_RX_BUS_HANDLER_T h_bus,
        RxInt_RxHandler      rx_handler_func,
        RxInt_RxErrorHandler rx_error_func
    );

    /**
     * @brief  Starts the Rx interface.
     *         From this point on the Rx driver should operate autonomously (e.g. using interrupts,
     *         DMA or pulling with SW timers). Once a complete frame is received the driver should
     *         call the callback function to notify the a new frame has been captured.
     *
     * @param  h_bus Rx bus handler.
     *
     * @return DEF_TRUE if successful, DEF_FALSE otherwise.
     */
    bool_t (*RxInt_Start)(COM_RX_BUS_HANDLER_T h_bus);

    /**
     * @brief  Stops the Rx process.
     *
     * @param  h_bus Rx bus handler.
     */
    void (*RxInt_Stop)(COM_RX_BUS_HANDLER_T h_bus);

    /**
     * @brief Perform alignment.
     *        While testing it was noted that if the Rx driver is started in the middle of a
     *        frame, it will capture the frame misaligned. The alignment mechanism finds the
     *        correct header to start the reception at the correct time.
     *
     *        This function is called inside the RX ISR while performing the alignment. Once the
     *        alignment is completed, the Rx buffer should contain a valid frame.
     *
     * @param  p_buffer_info Pointer to the Rx buffer info struct.
     * @param  rx_byte Received byte.
     *
     * @return Result of the alignment process.
     */
    void (*RxInt_PerformAlignment)(RXINT_RX_INFO_T* p_rx_info, uint8_t rx_byte);

    /**
     * @brief  Translate the raw received frame into the standard frame representation.
     *
     * @param  p_buffer_info Pointer to the Rx buffer info struct.
     */
    void (*RxInt_ProcessFrame)(RXINT_RX_BUFFER_INFO_T* p_buffer_info, STD_FRAME_T* p_std_frame);

    /**
     * @brief  Implement some debugging on the received frame.
     *         The simplest debugging is to print the raw buffer through the debug interface,
     *         but the actual debug mechanism is left open based on the requirements at any given
     *         point.
     *         WARNING: Note 1.
     *
     * @param  p_buffer_info Pointer to the Rx buffer info struct.
     *
     * @return DEF_TRUE if successful, DEF_FALSE otherwise.
     *
     * @note List of notes:
     *       1. This function is optional, if set to NULL, it will not be executed without
     *          raising an error.
     */
    void (*RxInt_DebugFrame)(RXINT_RX_BUFFER_INFO_T* p_buffer_info);

} RXINT_INTERFACE_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/


/** @} (end addtogroup RxInterface) */
/** @} (end addtogroup RxDriver)    */
/** @} (end addtogroup Rc)          */
/** @} (end addtogroup App)         */

#endif /* __RX_INTERFACE_H__        */
