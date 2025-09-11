/**
 * @file  todo.h
 * @brief TODO
 *
 * @ingroup   Main
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __TODO_H__
#define __TODO_H__

#include "plt_types.h"


/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
********************************************************************************/

typedef struct PLT_MEMCA_HANDLER_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/**
 * @brief  Initialization function for the circular buffer allocator.
 *
 * @param  p_handler: Pointer to the handler that will be initialized.
 * @param  mempool:   Pointer to the memory region to manage, see note 1.
 * @param  chunk_size: Size of the memory chunks.
 * @param  n_chunks: Number of chunks in the memory region.
 *
 * @return DEF_TRUE if successful, DEF_FALSE otherwise.
 *
 * @note List of notes:
 *       1. The memory region must be chunk_size * n_chunks bytes long. This needs to be
 *          ensured by the application since it is not checked here.
 */
void PltMemCA_Init(
    PLT_MEMCA_HANDLER_T* p_handler,
    void*                mempool,
    uint16_t             chunk_size,
    uint16_t             n_chunks
);

/**
 * @brief  Allocate a memory chunk.
 *
 * @param  p_handler: Pointer to the circular allocator handler.
 *
 * @return Pointer to the allocated chunk or NULL if there are no free buffers.
 */
void* PltMemCA_Allocate(PLT_MEMCA_HANDLER_T* p_handler);

/**
 * @brief  Frees the specified chunk.
 *         The freed chunk must be the oldest allocated chunk, otherwise the function will raise
 *         an assert.
 *
 * @param  p_handler: Pointer to the circular allocator handler.
 * @param  p_chunk: Pointer to the chunk that will be deallocated.
 */
void PltMemCA_Free(PLT_MEMCA_HANDLER_T* p_handler, void* p_chunk);

#endif /* __TODO_H__       */
