#ifndef __PLUTO_CORE_SIGQUEUE_H__
#define __PLUTO_CORE_SIGQUEUE_H__

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>


typedef struct
{
    int signum;
    pid_t pid;
} PLUTO_CORE_SigQueueElement_t;

struct PLUTO_CORE_SigQueue
{
    PLUTO_CORE_SigQueueElement_t *elements;
    size_t n_elements;
    uint32_t read_index;
    uint32_t write_index;
};
typedef struct PLUTO_CORE_SigQueue* PLUTO_CORE_SigQueue_t;

PLUTO_CORE_SigQueue_t PLUTO_CORE_CreateSigQueue(size_t n_elements);
void PLUTO_CORE_DestroySigQueue(PLUTO_CORE_SigQueue_t *queue);

bool PLUTO_CORE_SigQueueTop(const PLUTO_CORE_SigQueue_t queue, PLUTO_CORE_SigQueueElement_t *element);
bool PLUTO_CORE_SigQueuePut(PLUTO_CORE_SigQueue_t queue, PLUTO_CORE_SigQueueElement_t element);
bool PLUTO_CORE_SigQueueGet(PLUTO_CORE_SigQueue_t queue, PLUTO_CORE_SigQueueElement_t *element);

#endif
