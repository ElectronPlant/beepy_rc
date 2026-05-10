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
#include "queue.h"
#include "task.h"

#include "target.h"


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

#define MOTION_NUM_MOTORS (TARGET_MOTOR_NUM)

#define MOTION_NUM_PARALLEL_QUEUE_REQUESTS (5U)
#define MOTION_TIMEOUT_MS                  (3000u) /* Time between motion updates */ //TODO update
#define MOTION_TIMEOUT_TICKS \
    ((MOTION_TIMEOUT_MS * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)

#define MOTION_NUM_SERVOS (TARGET_NUM_SERVO_PWM_CHANNELS)

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum MOTION_ACTIONS_E {
    MOTION_ACTION_ADJUST_MOVEMENT = 0,
    MOTION_ACTION_STOP,
    MOTION_ACTION_START,
} MOTION_ACTIONS_T;

typedef struct MOTION_SET_POINT_S {
    float32_t VLin; /**< Linear velocity  */
    float32_t VAng; /**< Angular velocity */
} MOTION_SET_POINT_T;

typedef struct MOTION_QUEUE_MSG_S {
    MOTION_ACTIONS_T Action;
    union {
        MOTION_SET_POINT_T SetPoint;
    } Payload;
} MOTION_QUEUE_MSG_T;

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static void Motion_TaskLoop(void);
static void Motion_TaskMain(PLT_UTILS_UNUSED void* parameters);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
TaskHandle_t         Motion_TaskHandle = NULL;
static QueueHandle_t Motion_RxQueueHandle = NULL;


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
        ok = Motor_Init((MOTOR_HANDLER_T)&Target_Motors[m]);
        if (DEF_FALSE == ok) {
            return DEF_FALSE;
        }
    }

    for (uint8_t m = 0; m < MOTION_NUM_SERVOS; m++) {
        ok = Servo_Init((SERVO_HANDLER_T)&Target_Servos[m]);
        if (DEF_FALSE == ok) {
            return DEF_FALSE;
        }
    }

    /* Init Queue */
    Motion_RxQueueHandle =
        xQueueCreate(MOTION_NUM_PARALLEL_QUEUE_REQUESTS, sizeof(MOTION_QUEUE_MSG_T));
    if (NULL == Motion_RxQueueHandle) {
        ok = DEF_FALSE;
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
static void Rc_WaitForSetPoint(void) {
    MOTION_QUEUE_MSG_T msg;
    if (pdPASS == xQueueReceive(Motion_RxQueueHandle, &msg, MOTION_TIMEOUT_TICKS)) {
        switch (msg.Action) {
            case MOTION_ACTION_ADJUST_MOVEMENT:
                // TODO make something with the new setpoint.
                (void)msg;
                break;
            default:
                PLT_UNREACHABLE;
                break;
        }
    } else {
        /* - No-op - */
    }
}

static void Motion_TaskLoop(void) {
    static bool_t     forward = DEF_TRUE;
    uint32_t          cnt;
    MOTOR_DIRECTION_T dir;
    static float32_t  angle = 0;

    printf("--------------\n");
    for (uint8_t m = 0; m < MOTION_NUM_MOTORS; m++) {
        printf("--- MOTOR %u ---\n", m);
        printf("Start drive\n");
        if (DEF_TRUE == forward) {
            Motor_SetSpeed((MOTOR_HANDLER_T)&Target_Motors[m], 70.0);
            printf("going forward\n");
        } else {
            Motor_SetSpeed((MOTOR_HANDLER_T)&Target_Motors[m], -70.0);
            printf("Going backwards\n");
        }
        Motor_GetEncoderCnt((MOTOR_HANDLER_T)&Target_Motors[m], &cnt, &dir);
        printf("Encoder Count %lu, %u\n", cnt, dir);
    }
    forward = !forward;
    printf("--------------\n");

    for (uint8_t m = 0; m < MOTION_NUM_SERVOS; m++) {
        Servo_SetAngle((SERVO_HANDLER_T)&Target_Servos[m], angle);
    }
    angle += 10.0f;
    if (angle > 180.0f) {
        angle = 0;
    }
    printf("angle: %ld\n", (int32_t)angle);

    Rc_WaitForSetPoint();
}

static void Motion_TaskMain(PLT_UTILS_UNUSED void* parameters) {
    /* Setup */
    for (uint8_t m = 0; m < MOTION_NUM_MOTORS; m++) {
        Motor_Start((MOTOR_HANDLER_T)&Target_Motors[m]);
    }

    for (uint8_t m = 0; m < MOTION_NUM_SERVOS; m++) {
        Servo_Start((SERVO_HANDLER_T)&Target_Servos[m]);
    }

    /* Loop */
    while (DEF_TRUE) {
        Motion_TaskLoop();
    }
}

/** @} (end addtogroup Motion)   */
