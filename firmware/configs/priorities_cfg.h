/**
 * @file      priorities_cfg.h
 * @brief     Config File for the system priorities.
 *
 * @ingroup   Configs
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PRIORITIES_CFG_H__
#define __PRIORITIES_CFG_H__

#include "FreeRTOSConfig.h"
#include "plt_types.h"

/********************************************************************************
 * IRQ priorities
 ********************************************************************************/
/** SysTick
 * @note The SysTick needs to have the lowest priority, so as not to delay the hardware IRQs.
 *       For more info about this: https://www.programmersought.com/article/169111516206/
 *       In this case it will be set to the second lowest priority, since the button presses
 *       can be delayed without problems.
 */
#define PRIORITIES_CFG_SYSTICK_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1U)

/** IRQ priorities
 *  @note To prevent FreeRTOS ConfigAsserts
 */
#define PRIORITIES_CFG_IRQ_MAX_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)

/** Button IRQ priorities */
#define PRIORITIES_CFG_BUTTON_IRQ_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY)

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * API
 ********************************************************************************/


#endif /* __PRIORITIES_CFG_H__       */
