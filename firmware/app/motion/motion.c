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
#include "encoder.h"
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
static void Motion_TaskMain(PLT_UTILS_UNUSED void* parameters);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
TaskHandle_t Motion_TaskHandle = NULL;

ENC_INSTANCE_T Motion_Encoders[TARGET_ENCODER_NUM] = {
    {
        .Status = ENC_STATUS_UNINITIALIZED,
        .Peripheral = &TargetEnc1,
    },
    {
        .Status = ENC_STATUS_UNINITIALIZED,
        .Peripheral = &TargetEnc2,
    },
    {
        .Status = ENC_STATUS_UNINITIALIZED,
        .Peripheral = &TargetEnc3,
    },
    {
        .Status = ENC_STATUS_UNINITIALIZED,
        .Peripheral = &TargetEnc4,
    },
};

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
    if (DEF_FALSE == ok) {
        return DEF_FALSE;
    }

    ok = Enc_Init(&Motion_Encoders[0]);
    if (DEF_FALSE == ok) {
        return DEF_FALSE;
    }

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
    static bool_t forward = DEF_TRUE;
    printf("--------------\n");
    printf("Start drive\n");
    if (DEF_TRUE == forward) {
        DrivePwm_SetDuty(DRIVE_PWM_CHANNELS_CH1, 0.0);
        DrivePwm_SetDuty(DRIVE_PWM_CHANNELS_CH2, 70.0);
        printf("going forward\n");
    } else {
        DrivePwm_SetDuty(DRIVE_PWM_CHANNELS_CH2, 0.0);
        DrivePwm_SetDuty(DRIVE_PWM_CHANNELS_CH1, 70.0);
        printf("Going backwards\n");
    }
    forward = !forward;
    // printf(
    //     "PWM: %d.%d\n",
    //     (uint16_t)Motion_TempPwm,
    //     (uint16_t)(Motion_TempPwm - (float32_t)(uint16_t)Motion_TempPwm) * 100
    // );
    printf("Encoder Count %lu\n", Enc_GetCount(&Motion_Encoders[0]));
    printf("--------------\n");

    vTaskDelay(3000); /* delay 300 ticks */
}

static void Motion_TaskMain(PLT_UTILS_UNUSED void* parameters) {
    /* Setup */
    DrivePwm_Start();
    Enc_StartEncoder(&Motion_Encoders[0]);

    /* Loop */
    while (DEF_TRUE) {
        Motion_TaskLoop();
    }
}

/** @} (end addtogroup Motion)   */
