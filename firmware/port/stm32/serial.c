/**
 * @file     serial.c
 * @brief    Generic implementation of Rx the serial interface.
 *
 * @ingroup   Stm32Serial
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Serial_
 *
 */

#include "plt_assert.h"
#include "plt_types.h"


#include "priorities_cfg.h"

#include "serial_port.h"
#include "target.h"

#include "serial.h"


/** @addtogroup Port
 *    @{
 */

/** @addtogroup Serial
 *    @{
 */

/** @addtogroup SerialStm32
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

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

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Serial_StopReceptionInternal(SERIAL_HANDLER_T ser);


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/
/**
 * @brief  STM32 port of the serial init function.
 */
bool_t Serial_Init(
    SERIAL_HANDLER_T ser,
    void (*rx_handler_func)(uint8_t),
    void (*error_handler_func)(void)
) {

    PLT_ASSERT(SERIAL_STATUS_UNINITIALIZED == ser->Status);

    /* Enable peripheral clock */
    ser->Peripheral->ClkEnFn_Ptr(ser->Peripheral->Clk);

    /* Init Tx GPIOs */
    if (DEF_TRUE == ser->Peripheral->TxAvailable) {
        ser->Peripheral->TxClkEnFn_Ptr(ser->Peripheral->TxClk);
        LL_GPIO_InitTypeDef tx_gpio_init_struct = {
            .Pin = ser->Peripheral->TxPin,
            .Mode = LL_GPIO_MODE_ALTERNATE,
            .Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH,
            .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
            .Pull = LL_GPIO_PULL_NO,
            .Alternate = ser->Peripheral->TxAlternateFunc,
        };
        LL_GPIO_Init(ser->Peripheral->TxPort, &tx_gpio_init_struct);
    }

    /* Init Rx GPIOs */
    if (DEF_TRUE == ser->Peripheral->TxAvailable) {
        ser->Peripheral->RxClkEnFn_Ptr(ser->Peripheral->RxClk);
        LL_GPIO_InitTypeDef rx_gpio_init_struct = {
            .Pin = ser->Peripheral->RxPin,
            .Mode = LL_GPIO_MODE_ALTERNATE,
            .Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH,
            .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
            .Pull = LL_GPIO_PULL_NO,
            .Alternate = ser->Peripheral->RxAlternateFunc,
        };
        LL_GPIO_Init(ser->Peripheral->RxPort, &rx_gpio_init_struct);
    }

    /* Enable Interrupt */
    NVIC_SetPriority(
        ser->Peripheral->IrqType,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), ser->IrqPriority, 0)
    );
    NVIC_EnableIRQ(ser->Peripheral->IrqType);

    /* Init USART */
    LL_USART_InitTypeDef usart_init_struct = {
        .BaudRate = ser->BaudRate,
        .DataWidth = LL_USART_DATAWIDTH_9B,
        .StopBits = LL_USART_STOPBITS_2,
        .Parity = LL_USART_PARITY_EVEN,
        .TransferDirection = LL_USART_DIRECTION_TX_RX,
        .HardwareFlowControl = LL_USART_HWCONTROL_NONE,
        .OverSampling = LL_USART_OVERSAMPLING_16,
    };
    LL_USART_Init(ser->Peripheral->Serial, &usart_init_struct);
    LL_USART_ConfigAsyncMode(ser->Peripheral->Serial);
    LL_USART_Enable(ser->Peripheral->Serial);

    /* Init Serial handler */
    ser->Status = SERIAL_STATUS_INITIALIZED;

    return DEF_TRUE;
}

/**
 * @brief  STM32 implementation of the start reception function.
 *
 * @note List of notes:
 *       1. WARNING: if the transmitter is sending data before the HAL_UART_Receive_IT is called
 *          immediately there will be an overrun issue. This is because the issue is not handled
 *          correctly by the HAL. See here for more info:
 *          https://community.st.com/t5/stm32cubemx-mcus/how-to-handle-hal-uart-error-ore/td-p/481259
 *          The issue is fixed by clearing the ORE flag before starting the interrupt reception.
 */
bool_t Serial_StartReception(SERIAL_HANDLER_T ser) {

    PLT_ASSERT(SERIAL_STATUS_INITIALIZED == ser->Status);
    PLT_ASSERT(NULL != ser->ErrorHandlerFunct_Ptr);
    PLT_ASSERT(NULL != ser->RxHandlerFunct_Ptr);
    PLT_ASSERT(DEF_FALSE == SERIAL_IS_RX_IRQ_ENABLED(ser->Peripheral->Serial));

    /* Start reception */
    LL_USART_ClearFlag_ORE(ser->Peripheral->Serial); /* See note 1 */
    LL_USART_EnableIT_RXNE(ser->Peripheral->Serial);
    ser->Status = SERIAL_STATUS_RUNNING;

    return DEF_TRUE;
}

/**
 * @brief  STM32 implementation of the serial stop function.
 */
void Serial_StopReception(SERIAL_HANDLER_T ser) {
    PLT_ASSERT(SERIAL_STATUS_RUNNING == ser->Status);
    PLT_ASSERT(DEF_TRUE == SERIAL_IS_RX_IRQ_ENABLED(ser->Peripheral->Serial));

    Serial_StopReceptionInternal(ser);

    ser->Status = SERIAL_STATUS_INITIALIZED;
}


/******************************************
 * Internal functions
 *******************************************/

/**
 * @brief  Stops the RX interrupt.
 */
static void Serial_StopReceptionInternal(SERIAL_HANDLER_T ser) {
    LL_USART_DisableIT_RXNE(ser->Peripheral->Serial);
}


/******************************************
 * IRQ handler
 ******************************************/

/**
 * @brief  STM32 implementation of the handle RX function.
 */
inline uint8_t Serial_HandleRx(SERIAL_HANDLER_T ser) {
    uint8_t rx_byte = SERIAL_READ_DATA(ser->Peripheral->Serial);
    // LL_USART_ClearFlag_RXNE(Serial_Handler.Instance_Ptr);
    return rx_byte;
}

/**
 * @brief STM32 implementation of the handle error function.
 *
 * @note List of notes:
 *      1. The process the handle the errors is just to clear the flag and flush the data.
 */
void Serial_HandleError(SERIAL_HANDLER_T ser) {
    /* See note 1. */
    (void)SERIAL_READ_DATA(ser->Peripheral->Serial);
    SERIAL_CLEAR_ERROR_FLAGS(ser->Peripheral->Serial);
}

/**
 * @brief  STM32 implementation of the error flag checker function.
 */
bool_t Serial_IsErrorFlagEnabled(SERIAL_HANDLER_T ser) {
    return SERIAL_IS_ERROR_FLAG_ENABLED(ser->Peripheral->Serial);
}

/**
 * @brief  STM32 implementation of the IRQ enabled function.
 */
bool_t Serial_IsIrqEnabled(SERIAL_HANDLER_T ser) {
    return SERIAL_IS_RX_IRQ_ENABLED(ser->Peripheral->Serial);
}


/** @} (end addtogroup SerialStm32) */
/** @} (end addtogroup Serial)      */
/** @} (end addtogroup Port)        */
