/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "FreeRTOS.h"
#include "string.h"
#include "task.h"


/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void        SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void exampleTask(void *parameters);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
#include "stdio.h"
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len) {
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        ITM_SendChar(*ptr++);
    }
    return len;
}

#include "main.h"
#include "plt_assert.h"
#include "sbus.h"
#include "serial.h"


static SBUS_STATUS_T TestSbus;
TaskHandle_t         TaskHandle;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* Reset of all peripherals, Initializes the Flash interface and the Systick.
     */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* System interrupt init*/
    /* https://www.freertos.org/Documentation/02-Kernel/03-Supported-devices/04-Demos/ARM-Cortex/RTOS-Cortex-M3-M4
     */
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    bool_t ok = Sbus_Init();
    PLT_ASSERT(DEF_TRUE == ok);

    BaseType_t task_ok = xTaskCreate(
        exampleTask,
        "main",
        configMINIMAL_STACK_SIZE * 5,
        NULL,
        configMAX_PRIORITIES - 1U,
        &TaskHandle
    );
    PLT_ASSERT(pdPASS == task_ok);

    vTaskStartScheduler();
    PLT_UNREACHABLE;
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
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

    /* Update the time base */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) { // TODO avoid the use of high-level HAL here.
        Error_Handler();
    }
    LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */

    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    /**/
    LL_GPIO_ResetOutputPin(LD2_GPIO_Port, LD2_Pin);

    /**/
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);

    /**/
    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    /**/
    LL_GPIO_SetPinPull(B1_GPIO_Port, B1_Pin, LL_GPIO_PULL_NO);

    /**/
    LL_GPIO_SetPinMode(B1_GPIO_Port, B1_Pin, LL_GPIO_MODE_INPUT);

    /**/
    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) { // TODO use PLT_ASSERT for this.
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {}
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
       line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


/******************************************
 * FreeRTOS
 ******************************************/
// TODO: complete
void vApplicationTickHook(void) {}

void vApplicationMallocFailedHook(void) {
    taskDISABLE_INTERRUPTS();
    PLT_UNREACHABLE;
}

// TODO check if needed.
void vApplicationIdleHook(void) {}

/**
 * @brief  Stack overflow hook // TODO: implement
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    /* Check pcTaskName for the name of the offending task,
     * or pxCurrentTCB if pcTaskName has itself been corrupted. */
    (void)xTask;
    (void)pcTaskName;
}

/**
 * @brief  Example task to test FreeRTOS
 *
 * @param  parameters pointer to the task parameters.
 *
 */
static void exampleTask(void *parameters) {
    /* Unused parameters. */
    (void)parameters;

    for (;;) {
        /* --- Sbus Beging RX --- */
        printf("--------------\n");
        printf("Start reception\n");
        bool_t ok = Sbus_StartRx();
        PLT_ASSERT(DEF_TRUE == ok);

        LL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        vTaskDelay(1000); /* delay 100 ticks */

        /* --- SBUS check RX --- */
        Sbus_GetFrame(&TestSbus);
        printf("SBUS State: %u\n", TestSbus.State);
        if (SBUS_STATE_OK == TestSbus.State || SBUS_STATE_FRAME_LOST == TestSbus.State
            || SBUS_STATE_FAILSAFE == TestSbus.State) {
            Sbus_DebugFrame(TestSbus.FramePtr);
        } else {
            printf("Frame reception failed\n");
            printf("-----------------\n");
        }
    }
}

// TODO check this
void vApplicationGetIdleTaskMemory(
    StaticTask_t          **ppxIdleTaskTCBBuffer,
    StackType_t           **ppxIdleTaskStackBuffer,
    configSTACK_DEPTH_TYPE *puxIdleTaskStackSize
) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t  uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &(xIdleTaskTCB);
    *ppxIdleTaskStackBuffer = &(uxIdleTaskStack[0]);
    *puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}