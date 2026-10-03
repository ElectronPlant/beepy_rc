/**
 * @file  sound.h
 * @brief Module to play sounds using the board's buzzer.
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __SOUND_H__
#define __SOUND_H__

#include "plt_types.h"


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum SOUND_MELODIES_E {
    SOUND_MELODIES_INIT = 0,
    SOUND_MELODIES_ARM,
    SOUND_MELODIES_DISARM,
    SOUND_MELODIES_ATTENTION,
    SOUND_MELODIES_SAD,

    SOUND_MELODIES_MAX
} SOUND_MELODIES_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t Sound_Init(void);
void   Sound_StartMelody(SOUND_MELODIES_T melody_type);


#endif /* __SOUND_H__       */
