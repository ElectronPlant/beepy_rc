/**
 * @file  circular_alloc.c
 * @brief Circular memory allocator library.
 *
 * @ingroup   CircularAllocator
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
#include "plt_defines.h"
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

/********************************************************************************
 * Typedefs
 ********************************************************************************/

/********************************************************************************
 * Function Prototypes
 ********************************************************************************/
bool_t PltMemCA_AreChunksAvailable(PLTMEMCA_INSTANCE_T* p_instance);
void   PltMemCA_VerifyDeallocatedChunkAddress(PLTMEMCA_INSTANCE_T* p_instance, void* p_chunk);


/********************************************************************************
 * Local Vars
 ********************************************************************************/

/********************************************************************************
 * Function Implementations
 ********************************************************************************/

/**
 * @brief  Checks if there are chunks available.
 *
 * @param  p_instance: Pointer to the circular buffer allocator instance.
 *
 * @return DEF_TRUE if there are chunks available, DEF_FALSE otherwise.
 */
bool_t PltMemCA_AreChunksAvailable(PLTMEMCA_INSTANCE_T* p_instance) {
    return (p_instance->HeadIndex != p_instance->TailIndex || DEF_TRUE == p_instance->IsFree)
        ? DEF_TRUE
        : DEF_FALSE;
}

/**
 * @brief  Ensures that the deallocated chunk is correct.
 *
 * @param  p_instance: Pointer to the circular allocator instance.
 * @param  p_chunk: Pointer to the chunk that will be deallocated.
 */
void PltMemCA_VerifyDeallocatedChunkAddress(PLTMEMCA_INSTANCE_T* p_instance, void* p_chunk) {
    uint16_t offset = p_instance->ChunkSize * p_instance->TailIndex;
    void*    p_expected = (void*)((uint8_t*)p_instance->MemPool + offset);
    PLT_ASSERT(p_expected == p_chunk);
}


/******************************************
 * Interface
 ******************************************/

void PltMemCA_Init(
    PLTMEMCA_INSTANCE_T* p_instance,
    void*                mempool,
    uint16_t             chunk_size,
    uint16_t             n_chunks
) {
    p_instance->MemPool = mempool;
    p_instance->ChunkSize = chunk_size;
    p_instance->NumChunks = n_chunks;
    p_instance->HeadIndex = 0;
    p_instance->TailIndex = 0;
    p_instance->IsFree = DEF_TRUE;
}

void* PltMemCA_Allocate(PLTMEMCA_INSTANCE_T* p_instance) {
    void* ret = NULL;

    if (DEF_TRUE == PltMemCA_AreChunksAvailable(p_instance)) {
        uint16_t offset = p_instance->ChunkSize * p_instance->HeadIndex;
        ret = (void*)((uint8_t*)p_instance->MemPool + offset);
        p_instance->HeadIndex = (p_instance->HeadIndex + 1U) % p_instance->NumChunks;
        p_instance->IsFree = DEF_FALSE;
    }

    return ret;
}

void PltMemCA_Free(PLTMEMCA_INSTANCE_T* p_instance, void* p_chunk) {
    PltMemCA_VerifyDeallocatedChunkAddress(p_instance, p_chunk);
    p_instance->TailIndex = (p_instance->TailIndex + 1U) % p_instance->NumChunks;
    p_instance->IsFree = p_instance->TailIndex == p_instance->HeadIndex ? DEF_TRUE : DEF_FALSE;
}

#if PLT_DEFINES_USE_FREE_RTOS == 1
    #include "FreeRTOS.h"
    #include "task.h"

void* PltMemCA_AllocateCritical(PLTMEMCA_INSTANCE_T* p_instance) {
    taskENTER_CRITICAL();
    void* p_buffer = PltMemCA_Allocate(p_instance);
    taskEXIT_CRITICAL();
    return p_buffer;
}

void PltMemCA_FreeCritical(PLTMEMCA_INSTANCE_T* p_instance, void* p_chunk) {
    taskENTER_CRITICAL();
    PltMemCA_Free(p_instance, p_chunk);
    taskEXIT_CRITICAL();
}

void* PltMemCA_AllocateCriticalFromIsr(PLTMEMCA_INSTANCE_T* p_instance) {
    UBaseType_t interrupt_status;

    interrupt_status = taskENTER_CRITICAL_FROM_ISR();
    void* p_buffer = PltMemCA_Allocate(p_instance);
    taskEXIT_CRITICAL_FROM_ISR(interrupt_status);
    return p_buffer;
}

void PltMemCA_FreeCriticalFromIsr(PLTMEMCA_INSTANCE_T* p_instance, void* p_chunk) {
    UBaseType_t interrupt_status;

    interrupt_status = taskENTER_CRITICAL_FROM_ISR();
    PltMemCA_Free(p_instance, p_chunk);
    taskEXIT_CRITICAL_FROM_ISR(interrupt_status);
}
#endif /* USE_FREE_RTOS == 1 */

/** @} (end addtogroup Platform)            */
/** @} (end addtogroup Mem)                 */
/** @} (end addtogroup CircularAllocator)   */
