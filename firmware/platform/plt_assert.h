/**
 * @file     plt_assert.h
 * @brief    Assert implementation for the platform.
 *
 * @ingroup   PltAssert
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright (c) 2025 David Arnaiz
 *
 * @note Usage:
 *       The assert expect that the checked expression is True. Otherwise, they will be triggered.
 *       ```
 *       PLT_ASSERT(NULL != p_data);
 *       PLT_ASSERT(n < sizeof(array));
 *       PLT_ASSERT(DEF_TRUE == check_funct());
 *       ```
 *
 * @note credits:
 *   @li The asserting strategy follows the filename and line approach described by Tyler Hoffman
 *       in: https://interrupt.memfault.com/blog/asserts-in-embedded-systems.
 *       For the moment the approach is just to log the filename and line of the triggered assert.
 *       Ideally, the system should store the assert infor in memory and safely reboot.
 *   @li Some of the assert macros defined are inspired by the macros defined in the Rust's std
 *       (e.g. unimplemented!() and unreachable!())
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PLT_ASSERTS_H__
#define __PLT_ASSERTS_H__

#include "plt_types.h"

/** \addtogroup Plt
 *   @{
 */

/** \addtogroup PltAssert
 *   @{
 */

/********************************************************************************
 * Functions
 ********************************************************************************/
/**
 * @brief  Prints an error message with the file and line number of the triggered assert through
 *         the ESP_LOG interface.
 *
 * @param  file Pointer to the file name buffer.
 * @param  line Line of the triggered assert.
 */
void PltAssert_Assert(const char* file, uint32_t line);

/**
 * @brief  Prints a warning message with the file and line number of the triggered assert through
 *         the ESP_LOG interface.
 *
 * @param  file Pointer to the file name buffer.
 * @param  line Line of the triggered assert.
 */
void PltAssert_WarningAssert(const char* file, uint32_t line);

/********************************************************************************
 * Assert Macros
 ********************************************************************************/
#define PLT_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            PltAssert_Assert(__FILE__, __LINE__); \
        } \
    } while (0)

/**
 * @brief Produces an error if executed. This is usefull to specify branches that should never be
 *        reached.
 *
 * @note  Usage:
 *        ```
 *        ret = test_function();
 *        if (DEF_FALSE == ret) {
 *            PLT_UNREACHABLE;
 *        }
 *
 *        switch(test) {
 *            case case_1:
 *               // Do something
 *               break;
 *            ...
 *            default:
 *               // All cases should be covered
 *               PLT_UNREACHABLE;
 *               break;
 *        }
 */
#define PLT_UNREACHABLE PLT_ASSERT(DEF_FALSE)

/**
 * @brief Produces an warning message if the evaluated condition is false. Unlike asserts, it
 *        will not end the execution, and will log as a warning, not an error.
 *
 * @note Usage:
 *       ```
 *       PLT_WARNING_ASSERT(a < b);
 *       ```
 */
#define PLT_WARNING_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            PltAssert_WarningAssert(__FILE__, __LINE__); \
        } \
    } while (0)

/**
 * @brief Produces an warning message if executed. Unlike PLT_UNREACHABLE, it will not end the
 *        execution, and will log as a warning, not an error.
 *
 * @note Usage: same as PLT_UNREACHABLE
 */
#define PLT_WARNING_UNREACHABLE PLT_WARNING_ASSERT(DEF_FALSE)

/**
 * @brief Redefinition of PLT_WARNING_UNREACHABLE to explicitly state that a given part of the code
 *        is yet to be implemented.
 *
 * @note Usage:
 *       ```
 *       PLT_WARNING_ASSERT(a < b);
 *       ```
 */
#define PLT_UNIMPLEMENTED PLT_WARNING_UNREACHABLE

/********************************************************************************
 * Static Assert
 ********************************************************************************/
/**
 * @brief Forces a compilation error if the evaluated condition is not True.
 *        the build assert is used to evaluate conditions that do not need to be evaluated
 *        dynamically at runtime (as the regular asserts), but cannot be evaluated through
 *        preprocessor macros (e.g. sizeof()).
 *
 * @note The build assert needs to be placed as part of a function body, not the global scope. Also,
 *       the compiler error message is not very clear, so it needs to be well documented.
 *       ```
 *       int main (void) {
 *          // An error on the line below means that the buffer size needs to be increased.
 *          PLT_BUILD_ASSERT(sizeof(Message) < MAX_BUFFER_SIZE);
 *       }
 *       ```
 * @note Generated error message: "error: size of unnamed array is negative"
 *
 * @note Credits:
 *      @li The static assert implementation has been taken from the BUILD_BUG_ON implementation of
 *          the Linux kernel: https://lxr.linux.no/#linux+v2.6.26.5/include/linux/kernel.h#L494
 */
#define PLT_BUILD_ASSERT(expr) ((void)sizeof(char[1 - 2 * !(expr)]))


/** @} (end addtogroup PltAssert)   */
/** @} (end addtogroup Plt)         */

#endif /* __PLT_ASSERTS_H__         */
