/**
 * @file  buzzer.c
 *
 * @brief Driver for buzzers.
 *        Driver for the buzzer to play simple sounds. The buffer must not be directly driven from
 *        the MCU's GPIOs. Instead use a transistor to drive enough current through the buzzer.
 *
 * @ingroup   Buzzer
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Buzzer_
 */

#include "plt_assert.h"
#include "plt_defines.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "task.h"

#include "target.h"

#include "buzzer.h"
#include "pwm_timer.h"

/** @addtogroup Libs
 *   @{
 */

/** @addtogroup Buzzer
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define BUZZ_DEFAULT_FREQUENCY_HZ (100.0f)
#define BUZZ_HZ_TO_KHZ(X)         ((X) / 1000.0f)

#define BUZZ_DEFAULT_DUTY ((PWM_TIM_MIN_DUTY_CYCLE + PWM_TIM_MAX_DUTY_CYCLE) / 2.0f)
#define BUZZ_STOP_DUTY    (PWM_TIM_MIN_DUTY_CYCLE)


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
  * @brief  Initialize a buzzer.
  *
  * @param  buzz Buzzer handler.
  *
  * @return DEF_TRUE if successful, DEF_FALSE otherwise.
  */
bool_t Buzz_Init(BUZZER_HANDLER_T buzz) {
    bool_t ok;
    PLT_ASSERT(NULL != buzz);

    ok = PwmTim_InitChn(buzz->Timer, buzz->Chn, BUZZ_HZ_TO_KHZ(BUZZ_DEFAULT_FREQUENCY_HZ));
    if (DEF_FALSE == ok) {
        return DEF_FALSE;
    }
    return DEF_TRUE;
}

/**
 * @brief  Start a buzzer.
 *
 * @param  buzz Buzzer handler.
 */
void Buzz_Start(BUZZER_HANDLER_T buzz) {
    PLT_ASSERT(NULL != buzz);

    PwmTim_StartChn(buzz->Timer, buzz->Chn);
    PwmTim_SetDuty(buzz->Timer, buzz->Chn, BUZZ_DEFAULT_DUTY);
}

/**
 * @brief  Stop a buzzer.
 *
 * @param  buzz Buzzer handler.
 */
void Buzz_Stop(BUZZER_HANDLER_T buzz) {
    PLT_ASSERT(NULL != buzz);

    PwmTim_StopChn(buzz->Timer, buzz->Chn);
}

/**
 * @brief  Plays a tone on the buzzer.
 *
 * @param  buzz Buzzer handler.
 * @param  freq Frequency in Hz of the tone to play.
 */
void Buzz_PlayTone(BUZZER_HANDLER_T buzz, float32_t freq_hz) {
    PLT_ASSERT(NULL != buzz);

    PwmTim_SetTone(buzz->Timer, buzz->Chn, BUZZ_HZ_TO_KHZ(freq_hz));
}

/**
 * @brief  Stops the tone on the buzzer.
 *
 * @param  buzz Buzzer handler.
 *
 * @note List of notes:
 *      1. The buzzer is driven by an external transistor in open collector configuration. Thus,
 *         to stop current from flowing through the buzzer, the output must be set to 0.
 */
void Buzz_StopTone(BUZZER_HANDLER_T buzz) {
    PLT_ASSERT(NULL != buzz);
    PwmTim_StopTone(buzz->Timer, buzz->Chn);
}


/** @} (end addtogroup Buzzer)   */
/** @} (end addtogroup Libs)     */
