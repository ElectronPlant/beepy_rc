/**
 * @file     sbus.c
 * @brief    SBUS driver
 *
 * @ingroup   Sbus
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Sbus_
 *
 */

#include "stdio.h"
#include "string.h"

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "rx_interface.h"
#include "sbus.h"
#include "serial.h"
#include "std_frame.h"

/** @addtogroup Rc
 *   @{
 */

/** @addtogroup RxDriver
 *   @{
 */

/** @addtogroup Sbus
 *   @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
#define SBUS_HEADER_VALUE     (0x0F)
#define SBUS_FOOTER_VALUE     (0x00)
#define SBUS_BODY_START_INDEX (1U) /* Raw byte index of the frame (excluding header)*/
#define SBUS_HEADER_SIZE      (1U)
#define SBUS_BODY_SIZE        (SBUS_FRAME_SIZE_BYTES - 1U) /* Frame size (excluding header) */

/* SBUS ranges */
#define SBUS_CHN_MIN_VALUE (173U)
#define SBUS_CHN_MAX_VALUE (1812U)


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
} PLT_UTILS_PACKED SBUS_FRAME_T;


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void             Sbus_AlignmentWaitForHeader(RXINT_RX_INFO_T* p_rx_info, uint8_t rx_byte);
static bool_t           Sbus_SearchForNewHeader(RXINT_RX_INFO_T* p_rx_info);
static void             Sbus_AlignmentWaitForBuffer(RXINT_RX_INFO_T* p_rx_info, uint8_t rx_byte);
static bool_t           Sbus_IsFrameValid(SBUS_FRAME_T* p_frame);
static inline float32_t Sbus_Chn2Std(uint16_t v);
static void             Sbus_GetStdFrame(SBUS_FRAME_T* p_frame, STD_FRAME_T* p_std);

static bool_t Sbus_Init(RxInt_RxHandler rx_handler_func, RxInt_RxErrorHandler rx_error_func);
static bool_t Sbus_Start(void);
static void   Sbus_Stop(void);
static void   Sbus_PerformAlignment(RXINT_RX_INFO_T* p_rx_info, uint8_t rx_byte);
static void   Sbus_ProcessFrame(RXINT_RX_BUFFER_INFO_T* p_buffer_info, STD_FRAME_T* p_std_frame);
static void   Sbus_DebugFrame(RXINT_RX_BUFFER_INFO_T* p_buffer_info);

/********************************************************************************
 * Local Vars
 ********************************************************************************/
const RXINT_INTERFACE_T Sbus_Interface = {
    .RxInt_Init = Sbus_Init,
    .RxInt_Start = Sbus_Start,
    .RxInt_Stop = Sbus_Stop,
    .RxInt_PerformAlignment = Sbus_PerformAlignment,
    .RxInt_ProcessFrame = Sbus_ProcessFrame,
    .RxInt_DebugFrame = Sbus_DebugFrame,
};

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Waits for the header byte while performing the alignment.
 *
 * @param  p_rx_info Pointer to the Rx info.
 * @param  rx_byte Received byte.
 *
 * @note List of notes:
 *       1. There must be atleast one header byte per frame. If the number of bytes received
 *          while waiting for the header is larger than the frame size, the reception has failed.
 */
static void Sbus_AlignmentWaitForHeader(RXINT_RX_INFO_T* p_rx_info, uint8_t rx_byte) {
    if (SBUS_HEADER_VALUE == rx_byte) {
        /* Header frame found */
        p_rx_info->Count.RxCount = 0;
        p_rx_info->RxBufferInfo.RxBufferPtr[p_rx_info->Count.RxCount++] = rx_byte;
        p_rx_info->AlignmentStatus = RXINT_ALIGNMENT_STATUS_WAITING_FOR_FRAME;
    } else {
        p_rx_info->Count.HeaderCount++;
        if (p_rx_info->RxBufferInfo.RxBufferSize >= p_rx_info->Count.HeaderCount) {
            /* See note 1. */
            p_rx_info->AlignmentStatus = RXINT_ALIGNMENT_STATUS_FAILED;
        }
    }
}

/**
 * @brief  Checks for a new frame header in the current rx buffer.
 *
 * @param  p_rx_info Pointer to the Rx info.
 *
 * @return DEF_TRUE if a valid header is found, DEF_FALSE otherwise.
 */
static bool_t Sbus_SearchForNewHeader(RXINT_RX_INFO_T* p_rx_info) {
    bool_t   found = DEF_FALSE;
    uint16_t b = 1; /* Skip the current header */
    while (SBUS_HEADER_VALUE != p_rx_info->RxBufferInfo.RxBufferPtr[b]
           && b < SBUS_FRAME_SIZE_BYTES) {
        b++;
    };
    if (SBUS_FRAME_SIZE_BYTES < b) {
        p_rx_info->Count.RxCount = SBUS_FRAME_SIZE_BYTES - b;
        memcpy(
            p_rx_info->RxBufferInfo.RxBufferPtr,
            &p_rx_info->RxBufferInfo.RxBufferPtr[b],
            p_rx_info->Count.RxCount
        );
        found = DEF_TRUE;
    }
    return found;
}

/**
 * @brief  Waits for the a frame to be received while performing the alignment.
 *
 * @param  p_rx_info Pointer to the Rx info.
 * @param  rx_byte Received byte.
 *
 * @note List of notes:
 *       1. It is possible that the header byte is not correct since it is a valid byte value for
 *          other parts of the header. The validity of the frame has to be verified.
 *       2. If the header byte was not correctly picked, check the frame for any potential
 *          candidates, and continue the alignment from there. Since there is enough data for a
 *          complete frame, the real header must be present in the misaligned frame. Otherwise,
 *          the reception has failed.
 */
static void Sbus_AlignmentWaitForBuffer(RXINT_RX_INFO_T* p_rx_info, uint8_t rx_byte) {
    /* Store received frame */
    p_rx_info->RxBufferInfo.RxBufferPtr[p_rx_info->Count.RxCount++] = rx_byte;
    if (SBUS_FRAME_SIZE_BYTES >= p_rx_info->Count.RxCount) {
        /* The frame reception is complete, see note 1. */
        bool_t valid = Sbus_IsFrameValid((SBUS_FRAME_T*)p_rx_info->RxBufferInfo.RxBufferPtr);
        if (DEF_TRUE == valid) {
            /* The alignment has been successfully completed. */
            p_rx_info->AlignmentStatus = RXINT_ALIGNMENT_STATUS_COMPLETED;
        } else {
            /* See note 2. */
            uint8_t found = Sbus_SearchForNewHeader(p_rx_info);
            p_rx_info->AlignmentStatus = found == DEF_TRUE
                ? RXINT_ALIGNMENT_STATUS_WAITING_FOR_FRAME
                : RXINT_ALIGNMENT_STATUS_FAILED;
        }
    }
}

/**
 * @brief  Checks the validity of an SBUS frame.
 *
 * @param  p_frame: Pointer to the SBUS frame to check.
 *
 * @return DEF_TRUE if the frame is valid; DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. Frame integrity is checked ensuring that the header and footer values are correct.
 *          Further checks may be done in the future, but this is good enough for now.
 */
static bool_t Sbus_IsFrameValid(SBUS_FRAME_T* p_frame) {
    bool_t is_valid = DEF_FALSE;

    /* Validate Frame integrity, see note 1 */
    if (SBUS_HEADER_VALUE == p_frame->Header && SBUS_FOOTER_VALUE == p_frame->Footer) {
        is_valid = DEF_TRUE;
    }
    return is_valid;
}

/**
 * @brief  Gets the specified channel value from an SBUS frame.
 *
 * @param  p_frame: Pointer to the SBUS frame.
 * @param  chn: Number of the channel to return.
 *
 * @return SBUS frame channel value.
 */
static uint16_t Sbus_GetChannel(SBUS_FRAME_T* p_frame, uint8_t chn) {
    uint16_t value = 0;
    switch (chn) {
        case 1:
            value = p_frame->ServoChn.Chn1;
            break;
        case 2:
            value = p_frame->ServoChn.Chn2;
            break;
        case 3:
            value = p_frame->ServoChn.Chn3;
            break;
        case 4:
            value = p_frame->ServoChn.Chn4;
            break;
        case 5:
            value = p_frame->ServoChn.Chn5;
            break;
        case 6:
            value = p_frame->ServoChn.Chn6;
            break;
        case 7:
            value = p_frame->ServoChn.Chn7;
            break;
        case 8:
            value = p_frame->ServoChn.Chn8;
            break;
        case 9:
            value = p_frame->ServoChn.Chn9;
            break;
        case 10:
            value = p_frame->ServoChn.Chn10;
            break;
        case 11:
            value = p_frame->ServoChn.Chn11;
            break;
        case 12:
            value = p_frame->ServoChn.Chn12;
            break;
        case 13:
            value = p_frame->ServoChn.Chn13;
            break;
        case 14:
            value = p_frame->ServoChn.Chn14;
            break;
        case 15:
            value = p_frame->ServoChn.Chn15;
            break;
        case 16:
            value = p_frame->ServoChn.Chn16;
            break;
        default:
            PLT_UNREACHABLE;
    }
    return value;
}

/**
 * @brief  Transforms and SBUS channel to standard frame representation.
 *
 * @param  v: SBUS channel value to transform.
 *
 * @return Channel value in standard frame representation.
 */
static inline float32_t Sbus_Chn2Std(uint16_t v) {
    return PltUtils_MapF32(
        (float32_t)v,
        (float32_t)SBUS_CHN_MIN_VALUE,
        (float32_t)SBUS_CHN_MAX_VALUE,
        STD_FRAME_LOW_LIMIT,
        STD_FRAME_HIGH_LIMIT
    );
}

/**
 * @brief  Transforms an SBUS frame to standard frame representation.
 *
 * @param  p_frame: Pointer to the SBUS frame.
 * @param  p_std:   Pointer to where the standard frame will be stored.
 */
static void Sbus_GetStdFrame(SBUS_FRAME_T* p_frame, STD_FRAME_T* p_std) {
    memset(p_std, 0x00, sizeof(STD_FRAME_T));
    for (uint16_t chn = 0; chn < STD_FRAME_NUM_CHANNELS; chn++) {
        float32_t chn_value = 0.0f;
        if (chn < SBUS_NUM_CHANNELS) {
            uint16_t v = Sbus_GetChannel(p_frame, chn);
            chn_value = Sbus_Chn2Std(v);
        }
        p_std->Channels[chn] = chn_value;
    }

    STD_FRAME_STATE_T state = STD_FRAME_STATE_VALID;
    if (DEF_TRUE == p_frame->Flags.FailsafeEnabled) { /* Failsafe */
        state = STD_FRAME_STATE_FAILSAFE;
    } else if (DEF_TRUE == p_frame->Flags.FrameLost) { /* Frame lost */
        state = STD_FRAME_STATE_DROPPED;
    }
    p_std->State = state;
}


/******************************************
 * Driver interface
 ******************************************/
/**
 * @brief   SBUS implementation of the RxInt_Init interface.
 *
 * @note List of notes:
 *      1. Any compilation error pointing to this line means that the SBUS buffer size is not
 *         correct.
 *      2. The SBUS protocol is and inverted UART. This module assumes that the bus inversion is
 *         done at the hardware level. For the moment the UART peripheral is hardcoded.
 */
static bool_t Sbus_Init(RxInt_RxHandler rx_handler_func, RxInt_RxErrorHandler rx_error_func) {
    PLT_BUILD_ASSERT(sizeof(SBUS_FRAME_T) == SBUS_FRAME_SIZE_BYTES); /* Note 1 */

    bool_t init_ok;

    init_ok = Serial_Init(rx_handler_func, rx_error_func); /* See note 2 */ // TODO check notes.

    return init_ok;
}

/**
 * @brief   SBUS implementation of the RxInt_Start interface.
 */
static bool_t Sbus_Start(void) {
    return Serial_StartReception();
}

/**
 * @brief   SBUS implementation of the RxInt_Stop interface.
 */
static void Sbus_Stop(void) {
    Serial_StopReception();
}

/**
 * @brief   SBUS implementation of the RxInt_PerformAlignment interface.
 */
static void Sbus_PerformAlignment(RXINT_RX_INFO_T* p_rx_info, uint8_t rx_byte) {
    switch (p_rx_info->AlignmentStatus) {
        case RXINT_ALIGNMENT_STATUS_WAITING_FOR_HEADER:
            Sbus_AlignmentWaitForHeader(p_rx_info, rx_byte);
            break;
        case RXINT_ALIGNMENT_STATUS_WAITING_FOR_FRAME:
            Sbus_AlignmentWaitForBuffer(p_rx_info, rx_byte);
            break;
        default:
            PLT_UNREACHABLE;
            break;
    }
}

/**
 * @brief   SBUS implementation of the RxInt_ProcessFrame interface.
 */
static void Sbus_ProcessFrame(RXINT_RX_BUFFER_INFO_T* p_buffer_info, STD_FRAME_T* p_std_frame) {
    /* Sanitize the input */
    PLT_ASSERT(SBUS_FRAME_SIZE_BYTES <= p_buffer_info->RxBufferSize);
    PLT_ASSERT(NULL != p_buffer_info->RxBufferPtr);
    PLT_ASSERT(NULL != p_std_frame);

    SBUS_FRAME_T* p_frame = (SBUS_FRAME_T*)p_buffer_info->RxBufferPtr;

    bool_t valid = Sbus_IsFrameValid(p_frame);
    if (DEF_TRUE == valid) {
        Sbus_GetStdFrame((SBUS_FRAME_T*)p_frame, p_std_frame); //TODO fix typing here.
    } else {
        p_std_frame->State = STD_FRAME_STATE_INVALID;
    }
}

/**
 * @brief   SBUS implementation of the RxInt_DebugFrame interface.
 *          This implementation just prints the frame through the debug interface.
 */
static void Sbus_DebugFrame(RXINT_RX_BUFFER_INFO_T* p_buffer_info) {
    /* Sanitize the input */
    PLT_ASSERT(SBUS_FRAME_SIZE_BYTES <= p_buffer_info->RxBufferSize);
    PLT_ASSERT(NULL != p_buffer_info->RxBufferPtr);

    /* Debug frame */
    SBUS_FRAME_T* p_frame = (SBUS_FRAME_T*)p_buffer_info->RxBufferPtr;
    printf("------SBUS------\n");
    printf("Header: %u\n", p_frame->Header);
    printf("Servo Channels:\n");
    for (uint8_t c = 1; c <= SBUS_NUM_SERVO_CHANNELS; c++) {
        printf("    Chn %u: %d\n", c, Sbus_GetChannel(p_frame, c));
    }
    printf("Switch Channels:\n");
    printf("    Chn 16: %u\n", p_frame->Flags.Chn17);
    printf("    Chn 17: %u\n", p_frame->Flags.Chn18);
    printf(
        "Flags: Frame Lost (%u), Fail Safe (%u), Unused (%u)\n",
        p_frame->Flags.FrameLost,
        p_frame->Flags.FailsafeEnabled,
        p_frame->Flags.Unused
    );
    printf("Footer: %u\n", p_frame->Footer);
    printf("Raw Bytes:");
    for (uint8_t b = 0; b < SBUS_FRAME_SIZE_BYTES; b++) {
        printf(" 0x%02X", ((uint8_t*)p_frame)[b]);
    }
    printf("\n");
    printf("-----------------\n");
}

/** @} (end addtogroup Sbus)   */
/** @} (end addtogroup RxDriver)   */
/** @} (end addtogroup Rc)   */
