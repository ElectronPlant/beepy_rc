/**
 * @file      rc.c
 * @brief     Radio Control module - Rx from the radio.
 *
 * @ingroup   Rc
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note Module Prefix: Rc_
 *
 */
#include <string.h>

#include "circular_alloc.h"
#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "common_rx_interface.h"
#include "common_rx_sizes.h"
#include "model.h"
#include "rc_inputs.h"
#include "rx_interface.h"
#include "std_frame.h"

/** @addtogroup Rc
 *   @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
/* --- Task --- */
#define RC_TASK_NAME       ("Rc")
#define RC_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define RC_TASK_PRIORITY   (configMAX_PRIORITIES - 2U)

/* --- Rx --- */
#define RC_RX_TIMEOUT_MS    (300u) /* Time between two frames before the failsafe is triggered */
#define RC_RX_TIMEOUT_TICKS ((RC_RX_TIMEOUT_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)
#define RC_RX_ALIGNMENT_RETRIES (5)

/* --- Debug --- */
#define RC_DEBUG_RAW_FRAME (1) /* Set to 1 to enable RX raw frame debugging */

/* --- STD Frames --- */
#define RC_NUMBER_OF_PROCESSED_FRAMES (2U)

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum {
    RC_STATUS_UNINITIALIZED = 0,
    RC_STATUS_STOPPED,
    RC_STATUS_MISALIGNED,
    RC_STATUS_RUNNING,
    RC_STATUS_ERROR,
} RC_STATUS_T;

typedef enum {
    RC_ISR_STATUS_UNINITIALIZED = 0,
    RC_ISR_STATUS_ALIGNMENT_ONGOING,
    RC_ISR_STATUS_RUNNING,
    RC_ISR_STATUS_ERROR,
} RC_ISR_STATUS_T;

typedef enum {
    RC_ERROR_TYPES_UNKNOWN = 0,
    RC_ERROR_TYPES_RX_ERROR,
    RC_ERROR_TYPES_TIMEOUT,
    RC_ERROR_TYPES_ALIGNMENT_ERROR,
} RC_ERROR_TYPES_T;

typedef enum {
    RC_ACTION_STOP = 0,
    RC_ACTION_RX_COMPLETE,
    RC_ACTION_NOTIFY_ERROR,
    RC_ACTION_MAX,
} RC_ACTIONS_T;

typedef struct {
    RC_ACTIONS_T Action;
    union {
        RXINT_RX_BUFFER_INFO_T RxBufferInfo;
        RC_ERROR_TYPES_T       ErrorType;
    } Payload;
} RC_QUEUE_MSG_T;

typedef struct {
    RXINT_RX_BUFFER_INFO_T RxBufferInfo;
    uint16_t               RxCount; /**< Number of bytes in the Rx buffer */
} RC_RX_INFO_T;

typedef struct {
    RXINT_RX_INFO_T RxInfo;
    uint8_t         AlignmentRetries;
    RC_ISR_STATUS_T Status;
} RC_ISR_DATA_T;


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Rc_TaskLoop(void);
static void Rc_TaskMain(PLT_UTILS_UNUSED void* parameters);

static void Rc_UpdateStatus(RC_STATUS_T new_status);
static void Rc_RxComplete(RC_ISR_DATA_T* p_data);

static void Rc_RxHandler(uint8_t rx_byte);
static void Rc_RxErrorHandler(void);

/********************************************************************************
 * Local Vars
 ********************************************************************************/
static TaskHandle_t  Rc_TaskHandle = NULL;
static QueueHandle_t Rc_RxQueueHandle = NULL;

static const uint16_t Rc_RxBuffersNum = RXINT_MAX_PARALLEL_RAW_BUFFERS;
static uint8_t        Rc_RxBuffersPool[RXINT_MAX_PARALLEL_RAW_BUFFERS][COMRXS_BUFFER_SIZE];
static RC_STATUS_T    Rc_Status = RC_STATUS_UNINITIALIZED;

static STD_FRAME_T Rc_FailSafeFrame = {0};

/**
 * @note Access model: The buffer must only be allocated by the ISR, deallocation only be the Task.
 */
PLTMEMCA_INSTANCE_T Rc_RxBufferAllocator; /* See note. */

/**
 * @note Access model: The Reset flag must only be modified after stopping the ISR.
 */
volatile bool_t Rc_ResetIsr = DEF_TRUE; /* See note. */

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Initialize the Rc module.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Rc_Init(void) {
    bool_t ok = DEF_TRUE;

    PLT_ASSERT(RC_STATUS_UNINITIALIZED == Rc_Status); /* Guard double initialization */

    /* Init Queue */
    Rc_RxQueueHandle = xQueueCreate(RXINT_MAX_PARALLEL_RAW_BUFFERS, sizeof(RC_QUEUE_MSG_T));
    if (NULL == Rc_RxQueueHandle) {
        ok = DEF_FALSE;
    }

    /* Init Rx Driver */
    ComRxInt_CheckInterface();
    if (DEF_TRUE == ok) {
        ok = ComRxInt_Interface.RxInt_Init(ComRxBus_BusHandler, Rc_RxHandler, Rc_RxErrorHandler);
    }

    /* Init Buffer handler */
    if (DEF_TRUE == ok) {
        PltMemCA_Init(&Rc_RxBufferAllocator, Rc_RxBuffersPool, COMRXS_BUFFER_SIZE, Rc_RxBuffersNum);
    }

    /* Create Task */
    if (DEF_TRUE == ok) {
        BaseType_t task_ok = xTaskCreate(
            Rc_TaskMain,
            RC_TASK_NAME,
            RC_TASK_STACK_SIZE,
            NULL,
            RC_TASK_PRIORITY,
            &Rc_TaskHandle
        );
        ok = PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(task_ok);
    }

    if (DEF_TRUE == ok) {
        Rc_UpdateStatus(RC_STATUS_STOPPED);
    }

    return ok;
}

/******************************************
 * Task Main
 ******************************************/
/**
 * @brief  Handles a Rx Complete action request.
 *
 * @param  p_buffer_info: Pointer to the buffer information.
 */
static void Rc_ActionRxComplete(RXINT_RX_BUFFER_INFO_T* p_buffer_info) {
    STD_FRAME_T processed_frame;

#if RC_DEBUG_RAW_FRAME == 1
    if (NULL != ComRxInt_Interface.RxInt_DebugFrame) {
        ComRxInt_Interface.RxInt_DebugFrame(p_buffer_info);
    }
#endif

    ComRxInt_Interface.RxInt_ProcessFrame(p_buffer_info, &processed_frame);

    if (STD_FRAME_STATE_VALID == processed_frame.State) {
        if (RC_STATUS_MISALIGNED == Rc_Status || RC_STATUS_ERROR == Rc_Status) {
            Rc_UpdateStatus(RC_STATUS_RUNNING);
        }
        RcIn_HandleRcFrame(&processed_frame);
    } else if (STD_FRAME_STATE_INVALID == processed_frame.State) {
        memcpy(&processed_frame, &Rc_FailSafeFrame, sizeof(STD_FRAME_T));
        processed_frame.State = STD_FRAME_STATE_INVALID;
        Rc_UpdateStatus(RC_STATUS_MISALIGNED);
        Rc_ResetIsr = DEF_TRUE;
    }

    PltMemCA_FreeCritical(&Rc_RxBufferAllocator, p_buffer_info->RxBufferPtr);
}

/**
 * @brief  Handles a RX timeout action request.
 *         No RX has been received, handles the failsafe.
 */
static void Rc_ActionRxTimeout(void) {
    printf("RC - Frame dropped\n");

    /* Generate error frame */
    STD_FRAME_T error_frame;
    memcpy(&error_frame, &Rc_FailSafeFrame, sizeof(STD_FRAME_T));
    error_frame.State = STD_FRAME_STATE_DROPPED;

    /* Notify error */
    RcIn_HandleRcFrame(&error_frame);
}

/**
 * @brief  Handles a Rx error action request.
 *
 * @param  error: Type of the error that has taken place.
 */
static void Rc_ActionNotifyError(RC_ERROR_TYPES_T error) {
    printf("RC - Error %u\n", error);

    /* Restart RC */
    Rc_UpdateStatus(RC_STATUS_ERROR);
    ComRxInt_Interface.RxInt_Stop(ComRxBus_BusHandler);
    Rc_ResetIsr = DEF_TRUE;
    bool_t ok = ComRxInt_Interface.RxInt_Start(ComRxBus_BusHandler);
    PLT_ASSERT(DEF_TRUE == ok);
}

/**
  * @brief  Loop function for the RC task.
  */
static void Rc_TaskLoop(void) {
    RC_QUEUE_MSG_T msg;
    if (pdPASS == xQueueReceive(Rc_RxQueueHandle, &msg, RC_RX_TIMEOUT_TICKS)) {
        switch (msg.Action) {
            case RC_ACTION_RX_COMPLETE:
                Rc_ActionRxComplete(&msg.Payload.RxBufferInfo);
                break;
            case RC_ACTION_NOTIFY_ERROR:
                Rc_ActionNotifyError(msg.Payload.ErrorType);
                break;
            default:
                PLT_UNREACHABLE;
                break;
        }
    } else {
        Rc_ActionRxTimeout();
    }
}

/**
 * @brief  Start function for the RC task.
 */
static void Rc_TaskStart() {
    PLT_ASSERT(RC_STATUS_STOPPED == Rc_Status);

    bool_t ok = ComRxInt_Interface.RxInt_Start(ComRxBus_BusHandler);
    PLT_ASSERT(DEF_TRUE == ok);
    Rc_Status = RC_STATUS_MISALIGNED;
}

/**
 * @brief  RC task main function
 *
 * @param  parameters (unused)
 */
static void Rc_TaskMain(PLT_UTILS_UNUSED void* parameters) {
    /* Setup */
    Rc_TaskStart();

    /* Loop */
    while (DEF_TRUE) {
        Rc_TaskLoop();
    }
}

/******************************************
 * State Handling
 ******************************************/
/**
 * @brief  Updates the RC status.
 *
 * @param  new_status Status value to set.
 */
static void Rc_UpdateStatus(RC_STATUS_T new_status) {
    Rc_Status = new_status;
}

/******************************************
 * Queue
 ******************************************/
/**
 * @brief  Generic function to add action request to the RC task queue.
 *         Warning: This function must be called from within an ISR.
 *
 * @param  p_msg: Pointer to the message to be enqueued.
 */
static void Rc_NotifyFromIsr(RC_QUEUE_MSG_T* p_msg) {
    BaseType_t higher_priority_task_awoken = pdFALSE;

    /* Send Rx frame to the queue */
    BaseType_t queue_ok = xQueueSendFromISR(Rc_RxQueueHandle, p_msg, &higher_priority_task_awoken);
    PLT_ASSERT(pdPASS == queue_ok);

    portYIELD_FROM_ISR(higher_priority_task_awoken);
}

/******************************************
 * Rx ISR
 ******************************************/
/**
 * @brief  Stores the received byte in the RX buffer.
 *
 * @param  p_data: Pointer to the RX ISR data stuct with the buffer where the byte will be stored.
 * @param  rx_byte: Received Byte.
 *
 * @return DEF_TRUE if the RX buffer is full, DEF_FALSE otherwise.
 */
static bool_t Rc_StoreRxByte(RC_ISR_DATA_T* p_data, uint8_t rx_byte) {
    p_data->RxInfo.RxBufferInfo.RxBufferPtr[p_data->RxInfo.Count.RxCount++] = rx_byte;
    return p_data->RxInfo.RxBufferInfo.RxBufferSize <= p_data->RxInfo.Count.RxCount ? DEF_TRUE
                                                                                    : DEF_FALSE;
}

/**
 * @brief  Resets the ISR data struct when misalignment has been detected.
 *
 * @param  p_data: Pointer to the RX ISR data struct to be updated.
 */
static void Rc_FailAlignment(RC_ISR_DATA_T* p_data) {
    p_data->RxInfo.Count.HeaderCount = 0;
    p_data->AlignmentRetries++;
    p_data->RxInfo.AlignmentStatus = RXINT_ALIGNMENT_STATUS_WAITING_FOR_HEADER;
    if (RC_RX_ALIGNMENT_RETRIES <= p_data->AlignmentRetries) {
        RC_QUEUE_MSG_T msg = {
            .Action = RC_ACTION_NOTIFY_ERROR,
            .Payload.ErrorType = RC_ERROR_TYPES_ALIGNMENT_ERROR,
        };
        Rc_NotifyFromIsr(&msg);
    }
}

/**
 * @brief  Runs the RX alignment mechanism for the received Byte.
 *
 * @param  p_data: Pointer to the RX ISR data struct.
 * @param  rx_byte: Received Byte.
 *
 * @note List of notes:
 *       1. The buffer cannot be full if the alignment process is yet to be completed.
 *       2. The alignment process can only be completed when a complete frame is generated.
 */
static void Rc_PerformAlignment(RC_ISR_DATA_T* p_data, uint8_t rx_byte) {

    ComRxInt_Interface.RxInt_PerformAlignment(&p_data->RxInfo, rx_byte);

    switch (p_data->RxInfo.AlignmentStatus) {
        case RXINT_ALIGNMENT_STATUS_COMPLETED:
            /* See note 2 */
            PLT_ASSERT(p_data->RxInfo.RxBufferInfo.RxBufferSize <= p_data->RxInfo.Count.RxCount);

            p_data->Status = RC_ISR_STATUS_RUNNING;
            p_data->AlignmentRetries = 0;
            Rc_RxComplete(p_data);
            break;
        case RXINT_ALIGNMENT_STATUS_FAILED:
            Rc_FailAlignment(p_data);
            break;
        case RXINT_ALIGNMENT_STATUS_WAITING_FOR_HEADER:
        case RXINT_ALIGNMENT_STATUS_WAITING_FOR_FRAME:
            /* - No-op - */
            break;
        default:
            PLT_UNREACHABLE;
            break;
    }
}

/**
 * @brief  Completes the RX after a full buffer is received.
 *
 * @param  p_data: Pointer to the RX ISR data struct.
 */
static void Rc_RxComplete(RC_ISR_DATA_T* p_data) {
    RC_QUEUE_MSG_T msg = {
        .Action = RC_ACTION_RX_COMPLETE,
        .Payload.RxBufferInfo = p_data->RxInfo.RxBufferInfo
    };
    Rc_NotifyFromIsr(&msg);
    p_data->RxInfo.RxBufferInfo.RxBufferPtr = NULL;
    p_data->RxInfo.Count.RxCount = 0;
}

/**
 * @brief  Reads a Byte from the RX interface.
 *
 * @param  p_data: Pointer to the RX ISR data struct.
 * @param  rx_byte: Received Byte.
 */
static void Rc_HandleRxByte(RC_ISR_DATA_T* p_data, uint8_t rx_byte) {
    bool_t done = Rc_StoreRxByte(p_data, rx_byte);
    if (DEF_TRUE == done) {
        Rc_RxComplete(p_data);
    }
}

/**
 * @brief  Resets the RX ISR data struct.
 *
 * @param  p_data: Pointer to the RX ISR data struct to be reset.
 */
static void Rx_ResetRxIsrData(RC_ISR_DATA_T* p_data) {
    p_data->Status = RC_ISR_STATUS_ALIGNMENT_ONGOING;
    p_data->RxInfo.Count.HeaderCount = 0;
    p_data->RxInfo.RxBufferInfo.RxBufferSize = COMRXS_BUFFER_SIZE;
    p_data->RxInfo.AlignmentStatus = RXINT_ALIGNMENT_STATUS_WAITING_FOR_HEADER;
    p_data->AlignmentRetries = 0;
    Rc_ResetIsr = DEF_FALSE;
}

/**
 * @brief  Interrupt callback for the Rx.
 *
 * @param  rx_byte  Received byte.
 *
 */
static void Rc_RxHandler(uint8_t rx_byte) {
    static RC_ISR_DATA_T data = {.RxInfo.RxBufferInfo.RxBufferPtr = NULL};

    if (DEF_TRUE == Rc_ResetIsr) {
        Rx_ResetRxIsrData(&data);
    }

    if (NULL == data.RxInfo.RxBufferInfo.RxBufferPtr) {
        data.RxInfo.RxBufferInfo.RxBufferPtr =
            PltMemCA_AllocateCriticalFromIsr(&Rc_RxBufferAllocator);
    }

    switch (data.Status) {
        case RC_ISR_STATUS_ALIGNMENT_ONGOING:
            Rc_PerformAlignment(&data, rx_byte);
            break;
        case RC_ISR_STATUS_RUNNING:
            Rc_HandleRxByte(&data, rx_byte);
            break;
        default:
            /* - No-op - */
            break;
    }
}


/**
 * @brief  Rx Error Callback.
 *
 * @param  rx_byte  Received byte.
 *
 */
static void Rc_RxErrorHandler(void) {
    RC_QUEUE_MSG_T msg = {
        .Action = RC_ACTION_NOTIFY_ERROR,
        .Payload.ErrorType = RC_ERROR_TYPES_RX_ERROR,
    };
    Rc_NotifyFromIsr(&msg);
}

/** @} (end addtogroup Rc)   */
