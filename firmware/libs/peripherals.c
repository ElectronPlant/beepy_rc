/**
 * @file  peripherals.c
 * @brief Generic driver for the controllable peripherals.
 *        Controllable peripherals are all components that may be remotely controlled, which are
 *        not part of the main drive module (e.g. servos, lights, buzzer, etc.).
 *
 * @ingroup   Peripherals
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Per_
 */

#include "plt_assert.h"
#include "plt_types.h"

#include "target.h"

#include "gpio.h"
#include "servo.h"

#include "curves.h"
#include "peripherals.h"
#include "sound.h"
#include "ui.h"


/** @addtogroup Libs
 *    @{
 */

/** @addtogroup Peripherals
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
/**
 * @brief  Interface for the Rx protocols.
 *         This interface provides the abstraction layer for the different Rx protocols (e.g. SBUS).
 */
typedef struct {
    /**
     * @brief   Initializes the peripheral.
     *
     * @param  p_per Pointer to the peripheral to initialize.
     *
     * @return DEF_TRUE if successful, DEF_FALSE otherwise.
     */
    bool_t (*Init)(const PER_PERIPHERAL_T* p_per);

    /**
     * @brief  Starts the peripheral.
     *
     * @param  p_per Pointer to the peripheral to start.
     */
    void (*Start)(const PER_PERIPHERAL_T* p_per);

    /**
     * @brief  Applies the setpoint to peripheral.
     *
     * @param  p_per Pointer to the peripheral to control.
     * @param  setpoint Setpoint for the peripheral.
     */
    void (*ApplySetpoint)(const PER_PERIPHERAL_T* p_per, float32_t setpoint);

} PERIPHERAL_INTERFACE_T;


/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
static bool_t Per_InitAbsServo(const PER_PERIPHERAL_T* p_per);
static bool_t Per_InitRelServo(const PER_PERIPHERAL_T* p_per);
static bool_t Per_InitLight(const PER_PERIPHERAL_T* p_per);
static bool_t Per_InitPowerOff(const PER_PERIPHERAL_T* p_per);
static bool_t Per_InitBuzzer(const PER_PERIPHERAL_T* p_per);

static void Per_StartAbsServo(const PER_PERIPHERAL_T* p_per);
static void Per_StartRelServo(const PER_PERIPHERAL_T* p_per);
static void Per_StartLight(const PER_PERIPHERAL_T* p_per);
static void Per_StartPowerOff(const PER_PERIPHERAL_T* p_per);
static void Per_StartBuzzer(const PER_PERIPHERAL_T* p_per);

static void Per_ApplyAbsServoSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint);
static void Per_ApplyRelServoSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint);
static void Per_ApplyLightSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint);
static void Per_ApplyPowerOffSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint);
static void Per_ApplyBuzzerSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint);


/********************************************************************************
 * Local Vars
 ********************************************************************************/
PERIPHERAL_INTERFACE_T Per_Interface[PER_TYPE_MAX] = {
    {.Init = Per_InitAbsServo,
     .Start = Per_StartAbsServo,
     .ApplySetpoint = Per_ApplyAbsServoSetpoint},
    {.Init = Per_InitRelServo,
     .Start = Per_StartRelServo,
     .ApplySetpoint = Per_ApplyRelServoSetpoint},
    {.Init = Per_InitLight, .Start = Per_StartLight, .ApplySetpoint = Per_ApplyLightSetpoint},
    {.Init = Per_InitPowerOff,
     .Start = Per_StartPowerOff,
     .ApplySetpoint = Per_ApplyPowerOffSetpoint},
    {.Init = Per_InitBuzzer, .Start = Per_StartBuzzer, .ApplySetpoint = Per_ApplyBuzzerSetpoint},
};


/********************************************************************************
 * Function Implementations
 ********************************************************************************/
/******************************************
  * Init setpoint
  ******************************************/
/**
 * @brief  Init implementation for the absolute servo.
 */
static bool_t Per_InitAbsServo(const PER_PERIPHERAL_T* p_per) {
    if (TARGET_NUM_SERVOS <= p_per->Instance.Servos) {
        return DEF_FALSE;
    }
    return Servo_Init(Target_Servos[p_per->Instance.Servos]);
}

/**
 * @brief  Init implementation for the relative servo.
 */
static bool_t Per_InitRelServo(const PER_PERIPHERAL_T* p_per) {
    return Per_InitAbsServo(p_per);
}

/**
 * @brief  Init implementation for the lights.
 */
static bool_t Per_InitLight(const PER_PERIPHERAL_T* p_per) {
    if (TARGET_NUM_LIGHTS <= p_per->Instance.Lights) {
        return DEF_FALSE;
    }
    return Gpio_Init(Target_Lights[p_per->Instance.Lights]);
}

/**
 * @brief  Init implementation for the power off.
 */
static bool_t Per_InitPowerOff(const PER_PERIPHERAL_T* p_per) {
    /* - No-op - is enabled by the UI module. */
    return DEF_TRUE;
}

/**
 * @brief  Init implementation for the buzzer.
 */
static bool_t Per_InitBuzzer(const PER_PERIPHERAL_T* p_per) {
    /* - No-op - is enabled by the UI module. */
    return DEF_TRUE;
}

/**
 * @brief  Initializes the specified peripheral.
 *
 * @param  p_per Pointer to the peripheral to init.
 */
bool_t Per_Init(const PER_PERIPHERAL_T* p_per) {
    if (PER_TYPE_MAX <= p_per->Type || PER_TYPE_NONE == p_per->Type) {
        return DEF_FALSE;
    }
    return Per_Interface[p_per->Type - 1U].Init(p_per);
}

/******************************************
  * Start
  ******************************************/
/**
 * @brief  Start implementation for the absolute servo.
 */
static void Per_StartAbsServo(const PER_PERIPHERAL_T* p_per) {
    PLT_ASSERT(TARGET_NUM_SERVOS > p_per->Instance.Servos);
    Servo_Start(Target_Servos[p_per->Instance.Servos], p_per->Params.Servos.InitialSpan);
    Servo_SetSpanLimits(
        Target_Servos[p_per->Instance.Servos],
        p_per->Params.Servos.MaxSpan,
        p_per->Params.Servos.MinSpan
    );
}

/**
 * @brief  Start implementation for the relative servo.
 */
static void Per_StartRelServo(const PER_PERIPHERAL_T* p_per) {
    Per_StartAbsServo(p_per);
}

/**
 * @brief  Start implementation for the lights.
 *
 * @param  p_per Pointer to the peripheral to control.
 */
static void Per_StartLight(const PER_PERIPHERAL_T* p_per) {
    PLT_ASSERT(TARGET_NUM_LIGHTS > p_per->Instance.Lights);
    GPIO_VALUE_T v =
        DEF_FALSE == p_per->Params.Lights.StartEnabled ? GPIO_VALUE_HIGH : GPIO_VALUE_LOW;
    Gpio_Write(Target_Lights[p_per->Instance.Lights], v);
}

/**
 * @brief  Start implementation for the power off.
 *
 * @param  p_per Pointer to the peripheral to control.
 */
static void Per_StartPowerOff(const PER_PERIPHERAL_T* p_per) {
    /* - No-op - */
    (void)p_per;
}

/**
 * @brief  Start implementation for the buzzer.
 *
 * @param  p_per Pointer to the peripheral to control.
 */
static void Per_StartBuzzer(const PER_PERIPHERAL_T* p_per) {
    /* - No-op - */
    (void)p_per;
}


/**
 * @brief  Starts the specified peripheral.
 *
 * @param  p_per Pointer to the peripheral to start.
 */
void Per_Start(const PER_PERIPHERAL_T* p_per) {
    PLT_ASSERT(PER_TYPE_MAX > p_per->Type && PER_TYPE_NONE != p_per->Type);
    Per_Interface[p_per->Type - 1U].Start(p_per);
}


/******************************************
  * Apply setpoint
  ******************************************/
/**
 * @brief  ApplySetpoint implementation for the absolute servo.
 */
static void Per_ApplyAbsServoSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint) {
    PLT_ASSERT(TARGET_NUM_SERVOS > p_per->Instance.Servos);
    Servo_SetSpan(Target_Servos[p_per->Instance.Servos], setpoint);
}

/**
 * @brief  ApplySetpoint implementation for the relative servo.
 */
static void Per_ApplyRelServoSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint) {
    PLT_ASSERT(TARGET_NUM_SERVOS > p_per->Instance.Servos);
    Servo_SetRelSpan(Target_Servos[p_per->Instance.Servos], setpoint);
}

/**
 * @brief ApplySetpoint implementation for the lights.
 */
static void Per_ApplyLightSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint) {
    bool_t dig_setpoint = Curves_Analog2Dig(setpoint);

    PLT_ASSERT(TARGET_NUM_LIGHTS > p_per->Instance.Lights);
    Gpio_Write(Target_Lights[p_per->Instance.Lights], dig_setpoint);
}

/**
 * @brief ApplySetpoint implementation for the power off.
 */
static void Per_ApplyPowerOffSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint) {
    if (0.0f <= setpoint) {
        Ui_PowerOff();
    };
}

/**
 * @brief ApplySetpoint implementation for the buzzer.
 *
 * @note List of notes:
 *      1. When the switch for the buzzer is switched, it will replay the melody over and over
 *         again until switched back. This mechanism tracks that the buzzer was previously switched
 *         off until the sound can be repeated.
 */
static void Per_ApplyBuzzerSetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint) {
    static bool_t buzzer_running[PER_BUZZER_MAX] = {
        DEF_FALSE,
        DEF_FALSE,
    };

    if (0.0f <= setpoint && DEF_FALSE == buzzer_running[p_per->Instance.Buzzer]) {
        buzzer_running[p_per->Instance.Buzzer] = DEF_TRUE;

        SOUND_MELODIES_T melody;
        switch (p_per->Instance.Buzzer) {
            case PER_BUZZER_ATTENTION:
                melody = SOUND_MELODIES_ATTENTION;
                break;
            case PER_BUZZER_SAD:
                melody = SOUND_MELODIES_SAD;
                break;
            default:
                melody = SOUND_MELODIES_MAX;
        }
        if (SOUND_MELODIES_MAX != melody) {
            Sound_StartMelody(melody);
        }
    } else if (0.0f > setpoint) {
        buzzer_running[p_per->Instance.Buzzer] = DEF_FALSE;
    }
}


/**
 * @brief  Applies the setpoint to the specified peripheral.
 *
 * @param  p_per Pointer to the peripheral to control.
 * @param  setpoint Setpoint to apply.
 */
void Per_ApplySetpoint(const PER_PERIPHERAL_T* p_per, float32_t setpoint) {
    PLT_ASSERT(PER_TYPE_MAX > p_per->Type && PER_TYPE_NONE != p_per->Type);
    Per_Interface[p_per->Type - 1U].ApplySetpoint(p_per, setpoint);
}

/** @} (end addtogroup Peripherals)  */
/** @} (end addtogroup Libs)  */
