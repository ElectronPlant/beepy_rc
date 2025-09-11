/**
 * @file  circular_alloc.c
 * @brief Circular memory allocator library.
 *        This is used to allocate memory that will by deallocated in the same order they where
 *        deallocated.
 *        The main use is to pass long structs to queues and avoid the deep copy.RXINT_ERRORS_T.
 *
 * @note  This library is not reentrant, any protection needs to be done externally.
 *
 * @ingroup   CIRCULAR_ALLOCATOR
 * @version   V0.0
 * @author    David Arnaiz
 * @copyright 2025 David Arnaiz
 *
 * This file is part of BeepyRC <TODO: link to repo>.
 * This project is licensed under the GNU General Public License v3.0 license.
 *
 * @note    Module Prefix: PLT_MEMCA_
 */

#include "plt_assert.h"
#include "plt_types.h"

#include "circular_alloc.h"


/** @addtogroup Platform
 *    @{
 */

/** @addtogroup Mem
 *    @{
 */

/** @addtogroup CircularAllocator
 *    @{
 */


/********************************************************************************
 * Defines
 ********************************************************************************/
#define PLTMEMCA_VALIDATE_DEALLOCATED_BUFFER 0

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
    void*    MemPool;   /* Pointer to the memory region to manage, see note 1. */
    uint16_t ChunkSize; /* Size of the memory in bytes. */
    uint16_t NumChunks; /* Number of chunks available in the pool. */
    uint16_t HeadIndex; /* Index of the next free chunk. */
    uint16_t TailIndex; /* Index of the first used chunk. */
    bool_t   IsFree;    /* Flag to state if the buffer is empty, see note 2. */
} PLT_MEMCA_HANDLER_T;

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t PltMemCA_AreChunksAvailable(PLT_MEMCA_HANDLER_T* p_handler);
void   PltMemCA_VerifyDeallocatedChunkAddress(PLT_MEMCA_HANDLER_T* p_handler, void* p_chunk);


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Checks if there are chunks available.
 *
 * @param  p_handler: Pointer to the circular buffer allocator handler.
 *
 * @return DEF_TRUE if there are chunks available, DEF_FALSE otherwise.
 */
bool_t PltMemCA_AreChunksAvailable(PLT_MEMCA_HANDLER_T* p_handler) {
    return (p_handler->HeadIndex != p_handler->TailIndex || DEF_TRUE == p_handler->IsFree)
        ? DEF_TRUE
        : DEF_FALSE;
}

/**
 * @brief  Ensures that the deallocated chunk is correct.
 *
 * @param  p_handler: Pointer to the circular allocator handler.
 * @param  p_chunk: Pointer to the chunk that will be deallocated.
 */
void PltMemCA_VerifyDeallocatedChunkAddress(PLT_MEMCA_HANDLER_T* p_handler, void* p_chunk) {
    uint16_t offset = p_handler->ChunkSize * p_handler->TailIndex;
    void*    p_expected = (void*)((uint8_t*)p_handler->MemPool + offset);
    PLT_ASSERT(p_expected == p_chunk);
}


/******************************************
 * Interface
 ******************************************/

void PltMemCA_Init(
    PLT_MEMCA_HANDLER_T* p_handler,
    void*                mempool,
    uint16_t             chunk_size,
    uint16_t             n_chunks
) {
    p_handler->MemPool = mempool;
    p_handler->ChunkSize = chunk_size;
    p_handler->NumChunks = n_chunks;
    p_handler->HeadIndex = 0;
    p_handler->TailIndex = 0;
    p_handler->IsFree = DEF_TRUE;
}

void* PltMemCA_Allocate(PLT_MEMCA_HANDLER_T* p_handler) {
    void* ret = NULL;

    if (DEF_TRUE == PltMemCA_AreChunksAvailable(p_handler)) {
        uint16_t offset = p_handler->ChunkSize * p_handler->HeadIndex;
        ret = (void*)((uint8_t*)p_handler->MemPool + offset);
        p_handler->HeadIndex = (p_handler->HeadIndex + 1U) % p_handler->NumChunks;
        p_handler->IsFree = DEF_FALSE;
    }

    return ret;
}

void PltMemCA_Free(PLT_MEMCA_HANDLER_T* p_handler, void* p_chunk) {
    PltMemCA_VerifyDeallocatedChunkAddress(p_handler, p_chunk);
    p_handler->TailIndex = (p_handler->TailIndex + 1U) % p_handler->NumChunks;
    p_handler->IsFree = p_handler->TailIndex == p_handler->HeadIndex ? DEF_TRUE : DEF_FALSE;
}

/** @} (end addtogroup Platform)            */
/** @} (end addtogroup Mem)                 */
/** @} (end addtogroup CircularAllocator)   */
