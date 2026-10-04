/**
 * @file      bsp.c
 * @brief     Board Support Packet - Enabling basic components of the board.
 *            Two components are enabled (if available):
 *              @li The onboard LED that will be blinked to provide a heartbeat.
 *              @li The user button, which will trigger an interrupt. By default toggling the
 *                  onboard LED.
 *
 * @ingroup   Bsp
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
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

#include "priorities_cfg.h"

#include "target.h"

/** @addtogroup App
 *   @{
 */

/** @addtogroup Bsp
 *   @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/
#define BSP_LL_DRIVER_TO_APP_ERROR(X) (SUCCESS == X ? DEF_TRUE : DEF_FALSE)


/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/
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
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), PRIORITIES_CFG_SYSTICK_PRIORITY, 0)
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

    return ok;
}

/******************************************
 * Serial print
 ******************************************/
/**
 * @brief sends the printf strings through the SWD interface.
 */
int _write(int file, char* ptr, int len) {
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        ITM_SendChar(*ptr++);
    }
    return len;
}


/** @} (end addtogroup Bsp)   */
/** @} (end addtogroup App)   */
