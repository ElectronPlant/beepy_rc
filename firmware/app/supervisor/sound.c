/**
 * @file  sound.c
 * @brief Module to play sounds using the board's buzzer.
 *
 * @ingroup   Sound
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: Sound_
 */

#include "plt_assert.h"
#include "plt_types.h"
#include "plt_utils.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "target.h"

#include "sound.h"

#ifdef USE_BUZZER
    #include "buzzer.h"
#endif /* #ifdef USE_BUZZER */

/** @addtogroup Controller
 *    @{
 */

/** @addtogroup Sound
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
/* -- Timer -- */
#define SOUND_TIMER_HANDLE_NAME       ("Sound")
#define SOUND_DEFAULT_TIMER_PERIOD_MS (50U)
#define SOUND_TIMER_MS_TO_TICKS(X)    ((X * configTICK_RATE_HZ) / PLT_UTILS_SECS_TO_MS_FACTOR)
#define SOUND_TIMEOUT_MS              (1000u) /* Time between queue updates */
#define SOUND_TIMEOUT_TICKS           SOUND_TIMER_MS_TO_TICKS(SOUND_TIMEOUT_MS)


/********************************************************************************
 * Typedefs
 ********************************************************************************/

#ifdef USE_BUZZER

typedef struct SOUND_MELODY_TONE_S {
    uint16_t FreqHz;
    uint16_t DurationMs;
} SOUND_MELODY_TONE_T;

typedef struct SOUND_INFO_S {
    bool_t                     Initialized;
    volatile bool_t            MelodyPlaying;
    volatile uint16_t          MelodyIndex;
    volatile uint16_t          MelodyLength;
    const SOUND_MELODY_TONE_T* Melody_Ptr;
} SOUND_INFO_T;

#endif /* #ifdef USE_BUZZER */

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

#ifdef USE_BUZZER

static TimerHandle_t Sound_TimerHandle = NULL;

static SOUND_INFO_T Sound_Info =
    {.Initialized = DEF_FALSE, .MelodyPlaying = DEF_FALSE, .MelodyIndex = 0};

static const SOUND_MELODY_TONE_T Sound_MelodyInit[] = {
    {.FreqHz = 700, .DurationMs = 400},
    {.FreqHz = 300, .DurationMs = 50},
    {.FreqHz = 275, .DurationMs = 50},
    {.FreqHz = 250, .DurationMs = 50},
    {.FreqHz = 225, .DurationMs = 50},
    {.FreqHz = 200, .DurationMs = 50},
    {.FreqHz = 100, .DurationMs = 50},
    {.FreqHz = 0, .DurationMs = 400},
    {.FreqHz = 1000, .DurationMs = 25},
    {.FreqHz = 800, .DurationMs = 75},
    {.FreqHz = 1000, .DurationMs = 25},
    {.FreqHz = 0, .DurationMs = 300},
    {.FreqHz = 1000, .DurationMs = 25},
    {.FreqHz = 800, .DurationMs = 75},
    {.FreqHz = 1000, .DurationMs = 25},
};

static const SOUND_MELODY_TONE_T Sound_MelodyArm[] = {
    {.FreqHz = 1500, .DurationMs = 100},
    {.FreqHz = 2000, .DurationMs = 100},
    {.FreqHz = 1500, .DurationMs = 100},
};

static const SOUND_MELODY_TONE_T Sound_MelodyDisArm[] = {
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 400, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 400, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
};

static const SOUND_MELODY_TONE_T Sound_MelodyAttention[] = {
    {.FreqHz = 100, .DurationMs = 100}, {.FreqHz = 150, .DurationMs = 100},
    {.FreqHz = 200, .DurationMs = 100}, {.FreqHz = 250, .DurationMs = 100},
    {.FreqHz = 300, .DurationMs = 100}, {.FreqHz = 350, .DurationMs = 100},
    {.FreqHz = 400, .DurationMs = 100}, {.FreqHz = 450, .DurationMs = 100},
    {.FreqHz = 500, .DurationMs = 100}, {.FreqHz = 550, .DurationMs = 100},
    {.FreqHz = 600, .DurationMs = 100}, {.FreqHz = 650, .DurationMs = 100},
    {.FreqHz = 700, .DurationMs = 100}, {.FreqHz = 650, .DurationMs = 100},
    {.FreqHz = 600, .DurationMs = 100}, {.FreqHz = 550, .DurationMs = 100},
    {.FreqHz = 500, .DurationMs = 100}, {.FreqHz = 450, .DurationMs = 100},
    {.FreqHz = 500, .DurationMs = 100}, {.FreqHz = 550, .DurationMs = 100},
    {.FreqHz = 600, .DurationMs = 100}, {.FreqHz = 650, .DurationMs = 100},
    {.FreqHz = 700, .DurationMs = 100}, {.FreqHz = 800, .DurationMs = 100},
    {.FreqHz = 850, .DurationMs = 100}, {.FreqHz = 900, .DurationMs = 100},
    {.FreqHz = 950, .DurationMs = 100}, {.FreqHz = 1000, .DurationMs = 100},
    {.FreqHz = 0, .DurationMs = 250},   {.FreqHz = 1000, .DurationMs = 250},
    {.FreqHz = 0, .DurationMs = 250},   {.FreqHz = 800, .DurationMs = 250},
    {.FreqHz = 0, .DurationMs = 250},   {.FreqHz = 1000, .DurationMs = 250},
};

static const SOUND_MELODY_TONE_T Sound_MelodySad[] = {
    {.FreqHz = 200, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 200, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 200, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 200, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 200, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 200, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 300, .DurationMs = 100},
    {.FreqHz = 100, .DurationMs = 100},
    {.FreqHz = 300, .DurationMs = 100},
};


static const SOUND_MELODY_TONE_T* Sound_MelodySelector[SOUND_MELODIES_MAX] =
    {Sound_MelodyInit, Sound_MelodyArm, Sound_MelodyDisArm, Sound_MelodyAttention, Sound_MelodySad};

static const uint16_t Sound_MelodySizes[SOUND_MELODIES_MAX] = {
    PLT_UTILS_ARRAY_LENGTH(Sound_MelodyInit),
    PLT_UTILS_ARRAY_LENGTH(Sound_MelodyArm),
    PLT_UTILS_ARRAY_LENGTH(Sound_MelodyDisArm),
    PLT_UTILS_ARRAY_LENGTH(Sound_MelodyAttention),
    PLT_UTILS_ARRAY_LENGTH(Sound_MelodySad),
};

#endif /* #ifdef USE_BUZZER */


/********************************************************************************
 * Function Implementations
 ********************************************************************************/

#ifdef USE_BUZZER

/**
 * @brief  Non-blocking function to play the next tone of the melody.
 *         The tone contains the frequency of and the duration.
 * @note List of notes:
 *       1. If the timer fails to be set the software time, terminate the melody.
 */
void Sound_PlayNextTone(void) {
    const SOUND_MELODY_TONE_T* p_melody = Sound_Info.Melody_Ptr;
    if (Sound_Info.MelodyLength > Sound_Info.MelodyIndex) {
        if (0 == p_melody[Sound_Info.MelodyIndex].FreqHz) {
            Buzz_StopTone(Target_Buzzer);
        } else {
            Buzz_PlayTone(Target_Buzzer, (float32_t)(p_melody[Sound_Info.MelodyIndex].FreqHz));
        }
        BaseType_t tim_ret = xTimerChangePeriod(
            Sound_TimerHandle,
            SOUND_TIMER_MS_TO_TICKS(p_melody[Sound_Info.MelodyIndex].DurationMs),
            0
        );
        if (pdPASS != tim_ret) {
            /* See note 1. */
            Buzz_Stop(Target_Buzzer);
            Sound_Info.MelodyPlaying = DEF_FALSE;
        }
        Sound_Info.MelodyIndex++;
    } else {
        Buzz_Stop(Target_Buzzer);
        Sound_Info.MelodyPlaying = DEF_FALSE;
    }
}

/**
  * @brief  Callback function for the software timer.
  *
  * @param  xTimer Handle of the timer that expired.
  */
void Sound_TimerCallback(PLT_UTILS_UNUSED TimerHandle_t xTimer) {
    Sound_PlayNextTone();
}

/**
 * @brief  Initialize the sound generator.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Sound_Init(void) {
    bool_t ok = DEF_TRUE;

    if (DEF_TRUE == Sound_Info.Initialized) {
        ok = DEF_FALSE;
    }

    /* Init Timer */
    if (ok == DEF_TRUE) {
        Sound_TimerHandle = xTimerCreate(
            SOUND_TIMER_HANDLE_NAME,
            SOUND_TIMER_MS_TO_TICKS(SOUND_DEFAULT_TIMER_PERIOD_MS),
            pdFALSE,
            NULL,
            Sound_TimerCallback
        );
        ok = NULL != Sound_TimerHandle ? DEF_TRUE : DEF_FALSE;
    }

    /* Init Buzzer */
    if (DEF_TRUE == ok) {
        ok = Buzz_Init(Target_Buzzer);
    }

    /* Reset state */
    if (DEF_TRUE == ok) {
        Sound_Info.MelodyPlaying = DEF_FALSE;
        Sound_Info.Initialized = DEF_TRUE;
    }

    return ok;
}


/**
 * @brief  Starts playing the melody. See note 1.
 *
 * @param  melody_type Type of the melody to play.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. If a melody is already playing, the request to start a new melody will be ignored.
 *       2. From this point on, the melody will play using the software timer + callbacks. Once
 *          the melody is done, the buzzer will be stopped.
 */
void Sound_StartMelody(SOUND_MELODIES_T melody_type) {
    PLT_ASSERT(DEF_TRUE == Sound_Info.Initialized);
    PLT_ASSERT(SOUND_MELODIES_MAX > melody_type);

    if (DEF_FALSE == Sound_Info.MelodyPlaying) {
        Buzz_Start(Target_Buzzer);
        Sound_Info.MelodyPlaying = DEF_TRUE;
        Sound_Info.MelodyIndex = 0;
        Sound_Info.Melody_Ptr = Sound_MelodySelector[melody_type];
        Sound_Info.MelodyLength = Sound_MelodySizes[melody_type];
        Sound_PlayNextTone();
    } else {
        printf("Sound:: Ignoring melody %u, already runnning\n", melody_type);
    }
}

#else

/**
 * @brief  Initialize the sound generator.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
bool_t Sound_Init(void) {
    /* - No-op - */
    return DEF_TRUE;
}

/**
 * @brief  Starts playing the melody. See note 1.
 *
 * @param  melody_type Type of the melody to play.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 */
void Sound_StartMelody(PLT_UTILS_UNUSED SOUND_MELODIES_T melody_type) {
    /* - No-op - */
}

#endif /* #ifdef USE_BUZZER */

/** @} (end addtogroup Sound)       */
/** @} (end addtogroup Controller)  */
