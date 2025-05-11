
#include <pluto/pluto_core/data_layer/pluto_sig_queue.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <stdio.h>


PLUTO_CORE_SigQueue_t PLUTO_CORE_CreateSigQueue(size_t n_elements)
{
    PLUTO_CORE_SigQueue_t queue = PLUTO_Malloc(sizeof(struct PLUTO_CORE_SigQueue));

    queue->elements = PLUTO_Malloc(n_elements * sizeof(PLUTO_CORE_SigQueueElement_t));
    queue->n_elements = n_elements + 1;
    queue->read_index = 0U;
    queue->write_index = 0U;

    return queue;
}

void PLUTO_CORE_DestroySigQueue(PLUTO_CORE_SigQueue_t *queue)
{
    PLUTO_Free((*queue)->elements);
    PLUTO_Free(*queue);
    *queue = NULL;
}

bool PLUTO_CORE_SigQueueTop(const PLUTO_CORE_SigQueue_t queue, PLUTO_CORE_SigQueueElement_t *element)
{
    if(queue->read_index != queue->write_index)
    {
        *element = queue->elements[queue->read_index];
        return true;
    }
    return false;
}

bool PLUTO_CORE_SigQueuePut(PLUTO_CORE_SigQueue_t queue, PLUTO_CORE_SigQueueElement_t element)
{
    const uint32_t next_write_index = (queue->write_index + 1) % queue->n_elements;
    if(next_write_index != queue->read_index)
    {
        queue->elements[queue->write_index] = element;
        queue->write_index = next_write_index;
        return true;
    }
    return false;
}

bool PLUTO_CORE_SigQueueGet(PLUTO_CORE_SigQueue_t queue, PLUTO_CORE_SigQueueElement_t *element)
{
    if(queue->read_index != queue->write_index)
    {
        *element = queue->elements[queue->read_index];
        queue->read_index = (queue->read_index + 1) % queue->n_elements;
        return true;
    }
    return false;
}
