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

#include "encoder.h"
#include "motion.h"
#include "motor.h"
#include "pwm_timer.h"


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

#define MOTION_DEFAULT_MOTOR_PWM_FREQ_KHZ (20U)

#define MOTION_NUM_MOTORS (4U)

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

PWM_TIM_INSTANCE_T Motion_PwmTimers[TARGET_NUM_PWM_TIMERS] = {
    {
        .Status = PWM_TIM_STATUS_UNINITIALIZED,
        .Peripheral = &TargetMotorTim1,
        .EnChannels = 0x00,
        .InitChannels = 0x00,
    },
    {
        .Status = PWM_TIM_STATUS_UNINITIALIZED,
        .Peripheral = &TargetMotorTim2,
        .EnChannels = 0x00,
        .InitChannels = 0x00,
    },
    {
        .Status = PWM_TIM_STATUS_UNINITIALIZED,
        .Peripheral = &TargetMotorTim3,
        .EnChannels = 0x00,
        .InitChannels = 0x00,
    }
};

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

MOTOR_T Motion_Motors[MOTION_NUM_MOTORS] = {
    {
        .Encoder = &Motion_Encoders[0],
        .PwmChn[0] =
            {
                .Timer = &Motion_PwmTimers[0],
                .Chn = PWM_TIM_CHANNELS_CH1,
            },
        .PwmChn[1] =
            {
                .Timer = &Motion_PwmTimers[0],
                .Chn = PWM_TIM_CHANNELS_CH2,
            },
    },
    {
        .Encoder = &Motion_Encoders[1],
        .PwmChn[0] =
            {
                .Timer = &Motion_PwmTimers[0],
                .Chn = PWM_TIM_CHANNELS_CH4,
            },
        .PwmChn[1] =
            {
                .Timer = &Motion_PwmTimers[2],
                .Chn = PWM_TIM_CHANNELS_CH1,
            },
    },
    {
        .Encoder = &Motion_Encoders[2],
        .PwmChn[0] =
            {
                .Timer = &Motion_PwmTimers[1],
                .Chn = PWM_TIM_CHANNELS_CH1,
            },
        .PwmChn[1] =
            {
                .Timer = &Motion_PwmTimers[1],
                .Chn = PWM_TIM_CHANNELS_CH2,
            },
    },
    {
        .Encoder = &Motion_Encoders[3],
        .PwmChn[0] =
            {
                .Timer = &Motion_PwmTimers[1],
                .Chn = PWM_TIM_CHANNELS_CH3,
            },
        .PwmChn[1] = {
            .Timer = &Motion_PwmTimers[1],
            .Chn = PWM_TIM_CHANNELS_CH4,
        },
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

    for (uint8_t m = 0; m < MOTION_NUM_MOTORS; m++) {
        ok = Motor_Init((MOTOR_HANDLER_T)&Motion_Motors[m]);
        if (DEF_FALSE == ok) {
            return DEF_FALSE;
        }
    }

    /* Start Task */
    BaseType_t task_ok = xTaskCreate(
        Motion_TaskMain,
        MOTION_TASK_NAME,
        MOTION_TASK_STACK_SIZE,
        NULL,
        MOTION_TASK_PRIORITY,
        &Motion_TaskHandle
    );
    ok = PLT_UTILS_RTOS_TO_PLT_PASS_FAIL(task_ok);
    return ok;
}

/******************************************
 * Task Main
 ******************************************/
static void Motion_TaskLoop(void) {
    static bool_t     forward = DEF_TRUE;
    uint32_t          cnt;
    MOTOR_DIRECTION_T dir;

    printf("--------------\n");
    for (uint8_t m = 0; m < MOTION_NUM_MOTORS; m++) {
        printf("--- MOTOR %u ---\n", m);
        printf("Start drive\n");
        if (DEF_TRUE == forward) {
            Motor_SetSpeed((MOTOR_HANDLER_T)&Motion_Motors[m], 70.0);
            printf("going forward\n");
        } else {
            Motor_SetSpeed((MOTOR_HANDLER_T)&Motion_Motors[m], -70.0);
            printf("Going backwards\n");
        }
        forward = !forward;
        Motor_GetEncoderCnt((MOTOR_HANDLER_T)&Motion_Motors[m], &cnt, &dir);
        printf("Encoder Count %lu, %u\n", cnt, dir);
    }
    printf("--------------\n");

    vTaskDelay(3000); /* delay 300 ticks */
}

static void Motion_TaskMain(PLT_UTILS_UNUSED void* parameters) {
    /* Setup */
    for (uint8_t m = 0; m < MOTION_NUM_MOTORS; m++) {
        Motor_Start((MOTOR_HANDLER_T)&Motion_Motors[m]);
    }

    /* Loop */
    while (DEF_TRUE) {
        Motion_TaskLoop();
    }
}

/** @} (end addtogroup Motion)   */
