#include "pluto/os_abstraction/pluto_shared_memory.h"
#include <pluto/os_abstraction/message_queue/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_malloc.h>

//
// Only Build this if this Flag is set.
//
#ifdef PLUTO_MESSAGE_QUEUE_CUSTOM_0

#include <string.h>

PLUTO_MessageQueue_t PLUTO_CreateMessageQueue(
    const char *path, 
    const char *name, 
    unsigned int permissions,
    PLUTO_Logger_t logger
)
{
    (void)permissions;
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s-shm", name);
    PLUTO_SharedMemory_t shared_memory = PLUTO_CreateSharedMemory(
        4096,
        path,
        buffer,
        logger
    );
    if(!shared_memory)
    {
        return NULL;
    }
    snprintf(buffer, sizeof(buffer), "%s-sem", name);
    PLUTO_Semaphore_t semaphore = PLUTO_CreateSemaphore(
        path,
        buffer,
        logger
    );
    if(!semaphore)
    {
        PLUTO_DestroySharedMemory(&shared_memory);
        return NULL;
    }

    PLUTO_MessageQueue_t queue = PLUTO_Malloc(sizeof(struct PLUTO_MessageQueue));
    queue->internal = PLUTO_Malloc(sizeof(struct PLUTO_MessageQueueInternal));
    queue->internal->key = NULL;
    queue->internal->shared_memory = shared_memory;
    queue->data = (struct PLUTO_Data*) PLUTO_SharedMemoryAddress(shared_memory);
    /*
    printf("Shm obj addr: %p, %p\n", (void*)queue->data, shared_memory->address);
    printf("Shm write lock addr: %p\n", (void*)&(queue->data->header.write_lock));
    printf("Shm write addr: %p\n", (void*)&(queue->data->header.write_idx));
    printf("Shm read addr: %p\n", (void*)&(queue->data->header.read_idx));
    */
    atomic_store(
        &queue->data->header.read_idx,
        0
    );
    atomic_store(
        &queue->data->header.write_idx,
        0
    );
    atomic_store(
        &queue->data->header.write_lock,
        0
    );
    queue->internal->semaphore = semaphore;
    queue->internal->logger = logger;
    return queue;
}

PLUTO_MessageQueue_t PLUTO_MessageQueueGet(const char *path, const char *name, PLUTO_Logger_t logger)
{
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s-shm", name);
    PLUTO_SharedMemory_t shared_memory = PLUTO_CreateSharedMemory(
        4096,
        path,
        buffer,
        logger
    );
    if(!shared_memory)
    {
        printf("Queue shm error!\n");
        return NULL;
    }
    PLUTO_LoggerInfo(logger, "Created Queue mit Shm with %s%s", path, buffer);
    snprintf(buffer, sizeof(buffer), "%s-sem", name);
    printf("Queue create Sem!\n");
    PLUTO_Semaphore_t semaphore = PLUTO_SemaphoreGet(
        path,
        buffer,
        logger
    );
    if(!semaphore)
    {
        PLUTO_DestroySharedMemory(&shared_memory);
        return NULL;
    }

    printf("Queue create QUeueu!\n");
    PLUTO_MessageQueue_t queue = PLUTO_Malloc(sizeof(struct PLUTO_MessageQueue));
    queue->internal = PLUTO_Malloc(sizeof(struct PLUTO_MessageQueueInternal));
    queue->internal->key = NULL;
    queue->internal->shared_memory = shared_memory;
    queue->data = (struct PLUTO_Data*) PLUTO_SharedMemoryAddress(shared_memory);
    queue->internal->semaphore = semaphore;
    queue->internal->logger = logger;
    printf("------ QueueGet zuende\n");
    return queue;
}

void PLUTO_DestroyMessageQueue(PLUTO_MessageQueue_t *queue)
{
    PLUTO_DestroySharedMemory(&(*queue)->internal->shared_memory);
    PLUTO_DestroySemaphore(&(*queue)->internal->semaphore); 
    PLUTO_Free((*queue)->internal);
    PLUTO_Free(*queue);
    *queue = NULL;
}

bool PLUTO_MessageQueueRead(PLUTO_MessageQueue_t queue, PLUTO_Event_t event)
{
    unsigned int read_idx = atomic_load(
        &queue->data->header.read_idx
    ) & 0x1FU; // mod 31

    unsigned int write_idx = atomic_load(
        &queue->data->header.write_idx
    ) & 0x1FU; // mod 31
    
    if(write_idx == read_idx)
    {
        //
        // Queue empty...
        //
        return false;
    } 

    memcpy(
        (void*)event, 
        queue->data->data[read_idx].data, 
        sizeof(struct PLUTO_Event));

    atomic_store(
        &queue->data->header.read_idx,
        read_idx + 1U
    );
    
    return true;
}

static bool PLUTO_MessageQueueLockWrite(PLUTO_MessageQueue_t queue)
{
    unsigned int old_value = atomic_exchange(
        &queue->data->header.write_lock,
        1
    );
    return !old_value;
}

static void PLUTO_MessageQueueUnlockWrite(PLUTO_MessageQueue_t queue)
{
    atomic_store(
        &queue->data->header.write_lock,
        0
    ); 
}

bool PLUTO_MessageQueueWrite(PLUTO_MessageQueue_t queue, PLUTO_Event_t event)
{
    //
    // Multiple Writer...
    // Single Reader
    //
    // Implement critical Section for Writers:
    //      Writers must use a "Mutex" to lock Index increment.
    //

    //
    // Critical Section Start
    //
    if(!PLUTO_MessageQueueLockWrite(queue))
    {
        return false;
    }

    unsigned int write_idx = atomic_load(
        &queue->data->header.write_idx
    ) & 0x1FU; // 0x1FU = 31 -> write_idx modulo 31 
    unsigned int read_idx = atomic_load(
        &queue->data->header.read_idx
    ) & 0x1FU;

    if((write_idx + 1) == read_idx)
    {
        //
        // Queue full...
        //
        PLUTO_MessageQueueUnlockWrite(queue);
        return false;
    }
    
    memcpy(queue->data->data[write_idx].data, (const void*)event, sizeof(struct PLUTO_Event));
    atomic_store(
        &queue->data->header.write_idx,
        write_idx + 1
    );
    PLUTO_MessageQueueUnlockWrite(queue);
    //
    // Critical Section End
    //
    
    return true;
}

int32_t PLUTO_MessageQueueNumberOfMessagesAvailable(PLUTO_MessageQueue_t queue)
{
    (void)queue;
    return 0;
}

#endif
