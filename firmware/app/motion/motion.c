/**
 * @file      motion.c
 * @brief     Motion controller - handles movement.
 *
 * @ingroup   Rc
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note Module Prefix: Motion_
 *
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "FreeRTOS.h"
#include "task.h"

#include "target.h"

#include "drive_pwm.h"
#include "motion.h"


/** @addtogroup Motion
 *   @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define MOTION_TASK_NAME       ("Motion")
#define MOTION_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define MOTION_TASK_PRIORITY   (configMAX_PRIORITIES - 2U)

#define MOTION_TASK_DELAY_MS (300U)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Motion_TaskLoop(void);
static void Motion_TaskMain(PLT_UTILS_UNUSED void *parameters);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
TaskHandle_t Motion_TaskHandle = NULL;

float32_t Motion_TempPwm = 0;

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Initialize the motion module.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1.
 */
bool_t Motion_Init(void) {
    bool_t ok;

    ok = DrivePwm_Init();

    /* Start Task */
    if (DEF_TRUE == ok) {
        BaseType_t task_ok = xTaskCreate(
            Motion_TaskMain,
            MOTION_TASK_NAME,
            MOTION_TASK_STACK_SIZE,
            NULL,
            MOTION_TASK_PRIORITY,
            &Motion_TaskHandle
        );
        ok = PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(task_ok);
    }

    return ok;
}

/******************************************
 * Task Main
 ******************************************/
static void Motion_TaskLoop(void) {
    printf("--------------\n");
    printf("Start drive\n");
    Motion_TempPwm += 10.0;
    if (Motion_TempPwm > 100.0) {
        Motion_TempPwm = 0.0f;
    }
    DrivePwm_SetDuty(DRIVE_PWM_CHANNELS_CH1, Motion_TempPwm);
    printf(
        "PWM: %d.%d\n",
        (uint16_t)Motion_TempPwm,
        (uint16_t)(Motion_TempPwm - (float32_t)(uint16_t)Motion_TempPwm) * 100
    );
    printf("--------------\n");

    vTaskDelay(MOTION_TASK_DELAY_MS); /* delay 300 ticks */
}

static void Motion_TaskMain(PLT_UTILS_UNUSED void *parameters) {
    /* Setup */
    DrivePwm_Start();
    DrivePwm_SetDuty(DRIVE_PWM_CHANNELS_CH2, 50.0); // TODO tests.

    /* Loop */
    while (DEF_TRUE) {
        Motion_TaskLoop();
    }
}

/** @} (end addtogroup Motion)   */
