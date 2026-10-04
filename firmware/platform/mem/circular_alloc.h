/**
 * @file  circular_alloc.h
 * @brief Circular memory allocator library.
 *        This is used to allocate memory that will by deallocated in the same order they where
 *        deallocated.
 *        The main use is to pass long structs to queues and avoid the deep copy.
 *
 * @note  This library is not reentrant, any protection needs to be done externally.
 *
 * @ingroup   CircularAllocator
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC (https://github.com/ElectronPlant/beepy_rc).
 * This project is licensed under the GNU General Public License v3.0 license.
 */

#ifndef __PLTMEMCA_CIRCULAR_ALLOCATOR_H__
#define __PLTMEMCA_CIRCULAR_ALLOCATOR_H__

#include "plt_types.h"

/** @addtogroup Plt
 *    @{
 */

/** @addtogroup PltMem
 *    @{
 */

/** @addtogroup CircularAllocator
 *    @{
 */

/********************************************************************************
 * Defines
 ********************************************************************************/

/********************************************************************************
 * Typedefs
********************************************************************************/
/**
 * @note List of notes:
 *       1. See note 1 from PltMemCAlloc_Init.
 *       2. The header and tail indexes will look the same if all the memory is free or empty. Thus,
 *          this flag is used to distinguish between the two.
 */
typedef struct {
    void*    MemPool;   /**< Pointer to the memory region to manage, see note 1. */
    uint16_t ChunkSize; /**< Size of the memory in bytes. */
    uint16_t NumChunks; /**< Number of chunks available in the pool. */
    uint16_t HeadIndex; /**< Index of the next free chunk. */
    uint16_t TailIndex; /**< Index of the first used chunk. */
    bool_t   IsFree;    /**< Flag to state if the buffer is empty, see note 2. */
} PLTMEMCA_INSTANCE_T;


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/

/**
 * @brief  Initialization function for the circular buffer allocator.
 *
 * @param  instance: Pointer to the circular buffer allocator instance that will be initialized.
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
    PLTMEMCA_INSTANCE_T* p_instance,
    void*                mempool,
    uint16_t             chunk_size,
    uint16_t             n_chunks
);

/**
 * @brief  Allocate a memory chunk.
 *
 * @param  p_instance: Circular allocator handler.
 *
 * @return Pointer to the allocated chunk or NULL if there are no free buffers.
 */
void* PltMemCA_Allocate(PLTMEMCA_INSTANCE_T* p_instance);

/**
 * @brief  Frees the specified chunk.
 *         The freed chunk must be the oldest allocated chunk, otherwise the function will raise
 *         an assert.
 *
 * @param  p_instance: Pointer to the circular allocator instance.
 * @param  p_chunk: Pointer to the chunk that will be deallocated.
 */
void PltMemCA_Free(PLTMEMCA_INSTANCE_T* p_instance, void* p_chunk);

/******************************************
 * FreeRTOS enabled platforms
 ******************************************/
/**
 * @brief  PltMemCA_Allocate protected with critical sections.
 *         If used from withing an ISR use PltMemCA_AllocateCriticalFromIsr.
 */
void* PltMemCA_AllocateCritical(PLTMEMCA_INSTANCE_T* p_instance);

/**
 * @brief  PltMemCA_Free protected with critical sections.
 *         If used from withing an ISR use PltMemCA_FreeCriticalFromIsr.
 */
void PltMemCA_FreeCritical(PLTMEMCA_INSTANCE_T* p_instance, void* p_chunk);

/**
 * @brief  PltMemCA_Allocate protected with critical sections that can be used by an ISR..
 */
void* PltMemCA_AllocateCriticalFromIsr(PLTMEMCA_INSTANCE_T* p_instance);

/**
 * @brief  PltMemCA_Free protected with critical sections that can be used by an ISR.
 */
void PltMemCA_FreeCriticalFromIsr(PLTMEMCA_INSTANCE_T* p_instance, void* p_chunk);

/** @} (end addtogroup Plt)                 */
/** @} (end addtogroup PltMem)              */
/** @} (end addtogroup CircularAllocator)   */

#endif /* __PLTMEMCA_CIRCULAR_ALLOCATOR_H__       */
