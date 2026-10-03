/**
 * @file  supervisor_defines.h
 * @brief Generic defines for the supervisor task.
 *
 * @ingroup   Supervisor
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2026 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __SUPERVISOR_DEFINES_H__
#define __SUPERVISOR_DEFINES_H__

#include "plt_types.h"
#include "plt_utils.h"


/********************************************************************************
 * Defines
 ********************************************************************************/
#define SUPERDEF_SOURCE_BUTTONS_MASK \
    (PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_SOURCE_OFFSET_BUTTON_1) \
     | PLT_UTILS_BIT_OFFSET_TO_MASK(SUPERDEF_SOURCE_OFFSET_BUTTON_2))

/********************************************************************************
 * Typedefs
 ********************************************************************************/
typedef enum SUPERDEF_SOURCE_OFFSET_E {
    SUPERDEF_SOURCE_OFFSET_TIMER = 0,
    SUPERDEF_SOURCE_OFFSET_RC_ALIGNED,
    SUPERDEF_SOURCE_OFFSET_RC_ERROR,
    SUPERDEF_SOURCE_OFFSET_RC_DISCONNECTED,
    SUPERDEF_SOURCE_OFFSET_RC_RECONNECTED,
    SUPERDEF_SOURCE_OFFSET_ARMED,
    SUPERDEF_SOURCE_OFFSET_DISARMED,
    SUPERDEF_SOURCE_OFFSET_CONTROLLER_OK,
    SUPERDEF_SOURCE_OFFSET_CONTROLLER_ERROR,
    SUPERDEF_SOURCE_OFFSET_BUTTON_1,
    SUPERDEF_SOURCE_OFFSET_BUTTON_2,

    SUPER_SOURCE_MAX
} SUPERDEF_SOURCE_OFFSET_T;

typedef enum SUPERDEF_CONTEXT_OFFSET_E {
    SUPERDEF_CONTEXT_OFFSET_RC_ERROR = 0,
    SUPERDEF_CONTEXT_OFFSET_CONTROLLER_ERROR,
    SUPERDEF_CONTEXT_OFFSET_SUPERVISOR_ERROR,
    SUPERDEF_CONTEXT_OFFSET_RC_ALIGNED,
    SUPERDEF_CONTEXT_OFFSET_RC_CONNECTED,
    SUPERDEF_CONTEXT_OFFSET_ARMED,
} SUPERDEF_CONTEXT_OFFSET_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/**
  * @brief  Checks if there is any error active.
  *
  * @param  map State map.
  *
  * @return DEF_TRUE if there is any error active, DEF_FALSE otherwise.
  */
static inline bool_t SuperDef_IsAnyErrorSet(uint32_t map) {
    return PLT_UTILS_IS_BIT_OFFSET_SET(map, SUPERDEF_CONTEXT_OFFSET_RC_ERROR)
        || PLT_UTILS_IS_BIT_OFFSET_SET(map, SUPERDEF_CONTEXT_OFFSET_SUPERVISOR_ERROR)
        || PLT_UTILS_IS_BIT_OFFSET_SET(map, SUPERDEF_CONTEXT_OFFSET_CONTROLLER_ERROR);
}

#endif /* __SUPERVISOR_DEFINES_H__       */
