
#include <pluto_rw/prw_message_queue.h>

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


PLUTO_RW_MessageQueue_t PLUTO_RW_CreateMessageQueue(const char *name)
{
    key_t key = ftok(name, 1);
    int fd = msgget(key, 0);
    
    PLUTO_RW_MessageQueue_t queue = malloc(sizeof(struct PLUTO_RW_MessageQueue));
    queue->queue_handle = fd;
    if(fd < 0)
    {
        free(queue);
        queue = NULL;
    }
    return queue;
}

void PLUTO_RW_DestroyMessageQueue(PLUTO_RW_MessageQueue_t *queue)
{
    (void)queue;
}

bool PLUTO_RW_MessageQueueWrite(PLUTO_RW_MessageQueue_t queue, const PLUTO_RW_Message_t msg) 
{
    const int status = msgsnd(
        queue->queue_handle,
        &msg->buffer, 
        msg->size, 
        IPC_NOWAIT
    ); 
    if(status < 0)
    {
        if(EAGAIN != errno)
        {
            fprintf(stderr, "Error in Queue %i (errno: %i): %s\n", queue->queue_handle, errno, strerror(errno));
        }
        return false;
    }
    msg->size = status;
    return true;
} 

bool PLUTO_RW_MessageQueueRead(PLUTO_RW_MessageQueue_t queue, PLUTO_RW_Message_t msg)
{
    const int status = msgrcv(
        queue->queue_handle, 
        &msg->buffer, 
        sizeof(msg->buffer.text), 
        0, 
        IPC_NOWAIT
    );
    if(status < 0)
    {
        if(EAGAIN != errno)
        {
            fprintf(stderr, "Error in Queue: %s\n", strerror(errno));
        }
        return false;
    }
    msg->buffer.text[status] = '\0';
    msg->size = status;
    return true;
}
