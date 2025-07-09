/**
 * @file     serial.c
 * @brief    Generic implementation of the serial interface.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Serial_
 *
 */

#include "plt_assert.h"
#include "plt_types.h"


#include "serial.h"
#include "target.h"


/** @addtogroup HAL
 *   @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/* Packets are sent approximately every 10 ms or 20 ms, depending \
on the system configuration. */
#define SERIAL_BLOCKING_TIMEOUT_MS (20U)

#define SERIAL_APB1_PERIPH_CLOCK TARGET_RC_SERIAL_PERIPH_CLOCK
#define SERIAL_AHB1_GPIO_CLOCK   TARGET_RC_SERIAL_GPIO_CLOCK
#define SERIAL_INSTANCE          TARGET_RC_SERIAL_INSTANCE
#define SERIAL_INSTANCE_IRQ      TARGET_RC_SERIAL_IRQ
#define SERIAL_TX_GPIO_PIN       TARGET_RC_SERIAL_TX_PIN
#define SERIAL_RX_GPIO_PIN       TARGET_RC_SERIAL_RX_PIN
#define SERIAL_GPIO_PORT         TARGET_RC_SERIAL_PORT

#define SERIAL_BAUDRATE (100000U) // Real: (100000U) Fake: (115200)

/* --- TOOLS --- */

/* Translate between HAL status and internal bool error */
#define SERIAL_USART_ERROR_FLAGS (USART_SR_PE | USART_SR_FE | USART_SR_NE | USART_SR_ORE)
#define SERIAL_IS_ERROR_FLAG_ENABLED(USARTX) \
    ((0 == (READ_REG(USARTX->SR) & SERIAL_USART_ERROR_FLAGS)) ? DEF_FALSE : DEF_TRUE)
#define SERIAL_CLEAR_ERROR_FLAGS(USARTX) (WRITE_REG(USARTX->SR, ~SERIAL_USART_ERROR_FLAGS))
#define SERIAL_IS_RX_IRQ_ENABLED(USARTX) \
    ((0 == READ_BIT(USARTX->CR1, USART_CR1_RXNEIE)) ? DEF_FALSE : DEF_TRUE)
#define SERIAL_READ_DATA(USARTX) (USARTX->DR & 0x00FF)

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef struct {
    USART_TypeDef *Instance_Ptr;
    uint8_t       *RxBuffer_Ptr;
    uint16_t       RxBufferSize;
    uint16_t       RxByteCnt;
    void (*IntHandlerFunct_Ptr)(void);
} SERIAL_HANDLER_T;
/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Serial_HandleError(volatile SERIAL_HANDLER_T *p_handler);
static void Serial_NotifyRxComplete(volatile SERIAL_HANDLER_T *p_handler);
static void Serial_StopReceptionInternal(volatile SERIAL_HANDLER_T *p_handler);
static void Serial_HandleRx(volatile SERIAL_HANDLER_T *p_handler);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
static volatile SERIAL_HANDLER_T Serial_Handler = {0};

/********************************************************************************
 * Function Implementations
 ********************************************************************************/
/**
 * @brief
 *
 * @param  inp
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1.
 */
bool_t Serial_Init(void (*int_handler_fn)(void)) {
    /* Enable peripheral clock */
    LL_APB1_GRP1_EnableClock(SERIAL_APB1_PERIPH_CLOCK);
    LL_AHB1_GRP1_EnableClock(SERIAL_AHB1_GPIO_CLOCK);

    /* Init GPIOs */
    LL_GPIO_InitTypeDef gpio_init_struct = {
        .Pin = SERIAL_TX_GPIO_PIN | SERIAL_RX_GPIO_PIN,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = LL_GPIO_AF_8
    };
    LL_GPIO_Init(SERIAL_GPIO_PORT, &gpio_init_struct);

    /* Enable Interrupt */
    NVIC_SetPriority(SERIAL_INSTANCE_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(SERIAL_INSTANCE_IRQ);

    /* Init USART */
    LL_USART_InitTypeDef usart_init_struct = {
        .BaudRate = SERIAL_BAUDRATE,
        .DataWidth = LL_USART_DATAWIDTH_9B,
        .StopBits = LL_USART_STOPBITS_2,
        .Parity = LL_USART_PARITY_EVEN,
        .TransferDirection = LL_USART_DIRECTION_TX_RX,
        .HardwareFlowControl = LL_USART_HWCONTROL_NONE,
        .OverSampling = LL_USART_OVERSAMPLING_16,
    };
    LL_USART_Init(SERIAL_INSTANCE, &usart_init_struct);
    LL_USART_ConfigAsyncMode(SERIAL_INSTANCE);
    LL_USART_Enable(SERIAL_INSTANCE);

    /* Init Serial handler */
    Serial_Handler.Instance_Ptr = SERIAL_INSTANCE;
    Serial_Handler.RxByteCnt = 0;
    Serial_Handler.RxBufferSize = 0;
    Serial_Handler.RxBuffer_Ptr = NULL;
    Serial_Handler.IntHandlerFunct_Ptr = int_handler_fn;

    return DEF_TRUE;
}

/**
 * @brief
 *
 * @param  inp
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. WARNING: if the transmitter is sending data before the HAL_UART_Receive_IT is called
 *          immediately there will be an overrun issue. This is because the issue is not handled
 *          correctly by the HAL. See here for more info:
 *          https://community.st.com/t5/stm32cubemx-mcus/how-to-handle-hal-uart-error-ore/td-p/481259
 *          The issue is fixed by clearing the ORE flag before starting the interrupt reception.
 */
bool_t Serial_StartReception(uint8_t *p_data, uint16_t size) {
    /* Checks */
    PLT_ASSERT(NULL != p_data);
    PLT_ASSERT(0 != size);
    PLT_ASSERT(NULL != Serial_Handler.Instance_Ptr);
    if (DEF_TRUE == SERIAL_IS_RX_IRQ_ENABLED(Serial_Handler.Instance_Ptr)) {
        return DEF_FALSE;
    }

    /* Update Handler */
    Serial_Handler.RxBuffer_Ptr = p_data;
    Serial_Handler.RxBufferSize = size;
    Serial_Handler.RxByteCnt = 0;

    /* Start reception */
    LL_USART_ClearFlag_ORE(Serial_Handler.Instance_Ptr); /* See note 1 */ // TODO: Check if needed.
    LL_USART_EnableIT_RXNE(Serial_Handler.Instance_Ptr);

    return DEF_TRUE;
}

/**
 * @brief
 *
 * @param  inp
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1.
 */
void Serial_StopReception(void) {
    Serial_StopReceptionInternal(&Serial_Handler);
}


/******************************************
 * Internal functions
 *******************************************/

/**
 * @brief Handles error encountered while receiving data.
 *
 * @note List of notes:
 *      1. The process the handle the errors is just to clear the flag and flush the data.
 */
static void Serial_HandleError(volatile SERIAL_HANDLER_T *p_handler) {
    SERIAL_CLEAR_ERROR_FLAGS(p_handler->Instance_Ptr);
    (void)p_handler->Instance_Ptr->DR;
}

/**
 * @brief
 *
 * @param  inp
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1.
 */
static void Serial_NotifyRxComplete(volatile SERIAL_HANDLER_T *p_handler) {
    if (NULL != p_handler->IntHandlerFunct_Ptr) {
        p_handler->IntHandlerFunct_Ptr();
    }
}

/**
 * @brief
 *
 * @param  inp
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1.
 */
static void Serial_StopReceptionInternal(volatile SERIAL_HANDLER_T *p_handler) {
    LL_USART_DisableIT_RXNE(Serial_Handler.Instance_Ptr);
}

/**
 * @brief
 *
 * @param  inp
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1.
 */
static void Serial_HandleRx(volatile SERIAL_HANDLER_T *p_handler) {
    if (p_handler->RxByteCnt < p_handler->RxBufferSize) {
        p_handler->RxBuffer_Ptr[p_handler->RxByteCnt++] = SERIAL_READ_DATA(p_handler->Instance_Ptr);
        if (p_handler->RxByteCnt >= p_handler->RxBufferSize) {
            Serial_StopReceptionInternal(p_handler);
            Serial_NotifyRxComplete(p_handler);
        }
    }
    // LL_USART_ClearFlag_RXNE(Serial_Handler.Instance_Ptr);
}

/******************************************
 * IRQ handler
 ******************************************/

/**
 * @brief  System IRQ callback for the RC serial.
 */
void TARGET_RC_SERIAL_IRQ_HANDLER(void) {
    if (DEF_TRUE == SERIAL_IS_ERROR_FLAG_ENABLED(Serial_Handler.Instance_Ptr)) {
        Serial_HandleError(&Serial_Handler);
    } else if (DEF_TRUE == SERIAL_IS_RX_IRQ_ENABLED(Serial_Handler.Instance_Ptr)) {
        Serial_HandleRx(&Serial_Handler);
    }
}

/** @} (end addtogroup HAL)   */
