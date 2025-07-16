/**
 * @file      bsp.c
 * @brief     Board Support Packet - Enabling basic components of the board.
 *            Two components are enabled (if available):
 *              @li The onboard LED that will be blinked to provide a heartbeat.
 *              @li The user button, which will trigger an interrupt. By default toggling the
 *                  onboard LED.
 *
 * @ingroup   BSP
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note Module Prefix: BSP_
 *
 */

#include "stdio.h"

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "FreeRTOSConfig.h"
#include "timers.h"

#include "bsp_config.h"

#include "target.h"


/** @addtogroup BSP
 *   @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define BSP_LL_DRIVER_TO_APP_ERROR(X) (SUCCESS == X ? DEF_TRUE : DEF_FALSE)

#define BSP_HEARTBEAT_TIMER_NAME      ("HbeatT")
#define BSP_HEARTBEAT_TIMER_PERIOD_MS (1000) /* 1s */
#define BSP_TICK_PERIOD_MS            (portTICK_PERIOD_MS)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/* Callback function for the button interrupt. */
#ifdef TARGET_USE_BUTTON
static void (*BSP_ButtonCallbackFunct_Ptr)(void) = NULL;
#endif /* ifdef TARGET_USE_BUTTON */

#ifdef TARGET_USE_LED
static TimerHandle_t BSP_HbeatTimerHandler = NULL;
#endif /* ifdef TARGET_USE_LED */


/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/******************************************
 * Onboard LED
 ******************************************/

#ifdef TARGET_USE_LED

static void BSP_ToggleLed(void) {
    LL_GPIO_TogglePin(TARGET_LED_PORT, TARGET_LED_PIN);
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
static void BSP_HeartbeatCallback(PLT_UTILS_UNUSED TimerHandle_t p_handle) {
    BSP_ToggleLed();
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
bool_t BSP_SetupLed(void) {
    /* Enable Clocks */
    LL_AHB1_GRP1_EnableClock(TARGET_LED_GPIO_CLOCK);

    LL_GPIO_InitTypeDef gpio_init_struct = {
        .Pin = TARGET_LED_PIN,
        .Mode = LL_GPIO_MODE_OUTPUT,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO
    };
    LL_GPIO_Init(TARGET_LED_PORT, &gpio_init_struct);

    return BSP_LL_DRIVER_TO_APP_ERROR(SUCCESS);
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
bool_t BSP_SetupHeartbeat(void) {
    TickType_t period_ticks = BSP_HEARTBEAT_TIMER_PERIOD_MS / BSP_TICK_PERIOD_MS;

    BSP_HbeatTimerHandler = xTimerCreate(
        BSP_HEARTBEAT_TIMER_NAME, period_ticks, pdTRUE, NULL, BSP_HeartbeatCallback

    );
    bool_t ok = NULL == BSP_HbeatTimerHandler ? DEF_FALSE : DEF_TRUE;

    if (DEF_TRUE == ok) {
        ok = xTimerStart(BSP_HbeatTimerHandler, 0u);
    }

    return ok;
}

#endif /* ifdef TARGET_USE_LED */

/******************************************
 * Interrupt Button
 ******************************************/

#ifdef TARGET_USE_BUTTON

/**
 * @brief
 *
 * @param  inp
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. Priority just above the IDLE task
 */
bool_t BSP_SetupButton(void) {
    /* Enable Clocks */
    LL_AHB1_GRP1_EnableClock(TARGET_BUTTON_GPIO_CLOCK);

    LL_SYSCFG_SetEXTISource(TARGET_BUTTON_SYSCFG_EXTI_PORT, TARGET_BUTTON_SYSCFG_EXTI_LINE);

    LL_EXTI_InitTypeDef it_init_struct = {
        .Line_0_31 = TARGET_BUTTON_EXTI_LINE,
        .LineCommand = ENABLE,
        .Mode = LL_EXTI_MODE_IT,
        .Trigger = LL_EXTI_TRIGGER_FALLING
    };
    LL_EXTI_Init(&it_init_struct);
    LL_GPIO_SetPinPull(TARGET_BUTTON_PORT, TARGET_BUTTON_PIN, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinMode(TARGET_BUTTON_PORT, TARGET_BUTTON_PIN, LL_GPIO_MODE_INPUT);

    /* See note 1 */
    NVIC_SetPriority(
        TARGET_BUTTON_EXTI_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1u, 0)
    );
    NVIC_EnableIRQ(TARGET_BUTTON_EXTI_IRQ);

    return BSP_LL_DRIVER_TO_APP_ERROR(SUCCESS);
}

void TARGET_BUTTON_EXIT_IRQ_HANDLER(void) {
    if (LL_EXTI_ReadFlag_0_31(TARGET_BUTTON_EXTI_LINE) != RESET) {
        if (NULL != BSP_ButtonCallbackFunct_Ptr) {
            BSP_ButtonCallbackFunct_Ptr();
        }
        LL_EXTI_ClearFlag_0_31(TARGET_BUTTON_EXTI_LINE);
    }
}

void BSP_RegisterButtonCallback(void (*p_callback)(void)) {
    BSP_ButtonCallbackFunct_Ptr = p_callback;
    NVIC_EnableIRQ(TARGET_BUTTON_EXTI_IRQ);
}

void BSP_ResetButtonCallback(void) {
    BSP_ButtonCallbackFunct_Ptr = NULL;
    NVIC_DisableIRQ(TARGET_BUTTON_EXTI_IRQ);
}

#endif /* ifdef TARGET_USE_BUTTON */

/******************************************
 * Definitions
 ******************************************/
/**
 * @brief System Clock Configuration
 * @retval None
 * //TODO provide abstraction for this
 */
void Bsp_InitSystemClock(void) {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2) {}
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE3);
    LL_PWR_DisableOverDriveMode();
    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_HSI_Enable();

    /* Wait till HSI is ready */
    while (LL_RCC_HSI_IsReady() != 1) {}
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_16, 336, LL_RCC_PLLP_DIV_4);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1) {}
    while (LL_PWR_IsActiveFlag_VOS() == 0) {}
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}
    LL_SetSystemCoreClock(84000000);

    /* Setup the systick interrupt */
    uint32_t ticks = configCPU_CLOCK_HZ / configTICK_RATE_HZ; /* # of ticks between interrupts */
    SysTick_Config(ticks);
    NVIC_SetPriority(
        SysTick_IRQn,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), BSP_CONFIG_SYSTICK_PRIORITY, 0)
    );
    NVIC_EnableIRQ(SysTick_IRQn);

    LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/******************************************
 * Main functions
 ******************************************/

/**
 * @brief  Initialize the heartbeat task.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. System interrupt init needs to be set to group 4, see:
 *          https://www.freertos.org/Documentation/02-Kernel/03-Supported-devices/04-Demos/ARM-Cortex/RTOS-Cortex-M3-M4
 */
bool_t BSP_Init(void) {
    bool_t ok = DEF_TRUE;

    /* Reset of all peripherals, Initializes the Flash interface and the Systick.
     */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); /* See note 1 */

    /* Setup system clock */
    Bsp_InitSystemClock();

    /* Setup Hearbeat LED */

#ifdef TARGET_USE_LED
    ok = BSP_SetupLed();
    if (DEF_TRUE == ok) {
        ok = BSP_SetupHeartbeat();
    }
#endif /* ifdef TARGET_USE_LED */

    /* Setup Button */
#ifdef TARGET_USE_BUTTON
    if (DEF_TRUE == ok) {
        ok = BSP_SetupButton();
    }
    if (DEF_TRUE == ok) {
#ifdef TARGET_USE_LED
        BSP_RegisterButtonCallback(BSP_ToggleLed);
#else
        BSP_ResetButtonCallback();
#endif /* ifdef TARGET_USE_LED */
    }
#endif /* ifdef TARGET_USE_BUTTON */

    return ok;
}

/******************************************
 * Serial print
 ******************************************/
/**
 * @brief sends the printf strings through the SWD interface.
 */
int _write(int file, char *ptr, int len) {
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        ITM_SendChar(*ptr++);
    }
    return len;
}

/** @} (end addtogroup BSP)   */
