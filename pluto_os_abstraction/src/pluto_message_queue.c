
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_semaphore.h>
#include <pluto/os_abstraction/pluto_message_queue.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

#define PLUTO_MQ_INPUT_QUEUE_PERMISSIONS 0700 

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_MessageQueue_t PLUTO_CreateMessageQueue(const char *path, const char *name, unsigned int permissions)
{
    PLUTO_MessageQueue_t queue = (PLUTO_MessageQueue_t)PLUTO_Malloc(
        sizeof(struct PLUTO_MessageQueue)
    );

    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s-sem", name);
    queue->semaphore = PLUTO_CreateSemaphore(path, buffer);
    if(!queue->semaphore)
    {
        PLUTO_Free(queue);
        return NULL;
    }
    if(PLUTO_SEM_OK != PLUTO_SemaphoreSignal(queue->semaphore))
    {
        PLUTO_Free(queue);
        PLUTO_DestroySemaphore(&queue->semaphore);
        return NULL;
    }

    if(!PLUTO_CreateKey(path, name, &queue->key))
    {
        printf("Unable to create System V Key for Name: %s - %s\n", name, strerror(errno));
        return NULL;
    }
    const int identifier = msgget(
        queue->key.key, 
        IPC_CREAT | IPC_NOWAIT | MSG_NOERROR | PLUTO_MQ_INPUT_QUEUE_PERMISSIONS | permissions
    );
    queue->filedescriptor = identifier;  
    if(identifier < 0)
    {
        printf("Error, unable to create Message Queue...\n");
        PLUTO_Free(queue);
        return NULL;
    }
    return queue;
}

void PLUTO_DestroyMessageQueue(PLUTO_MessageQueue_t *queue)
{
    assert(NULL != *queue);
    if(*queue)
    {
        //
        // Decrement Reference Count.
        //
        PLUTO_SemaphoreWait((*queue)->semaphore);
        const int32_t semaphore_value = PLUTO_SemaphoreValue((*queue)->semaphore);
        printf("Queue %s Ref. Count %i\n", (*queue)->key.path_to_file, semaphore_value);
        if(semaphore_value <= 0)
        {
            printf("Destroy Queue %s\n", (*queue)->key.path_to_file);
            //
            // Only destroy Queue if the Reference Count indicates, 
            // that this is the last Instance which holds a Queue.
            //
            if(msgctl((*queue)->filedescriptor, IPC_RMID, NULL) < 0)
            {
                printf("Error, unable to delete MQ.\n");
            }
        }
        PLUTO_DestroySemaphore(&(*queue)->semaphore);
        PLUTO_DestroyKey((*queue)->key);
        PLUTO_Free(*queue);
        *queue = NULL;
    }
}

bool PLUTO_MessageQueueRead(PLUTO_MessageQueue_t queue, struct PLUTO_MsgBuf *buffer)
{
    assert(NULL != queue);
    
    long msgtype = 0;
    int msgflags = 0;
    buffer->msgtype = 1;
    const int nbytes = msgrcv(queue->filedescriptor, buffer, sizeof(buffer->text) - 1, msgtype, msgflags);
    if(nbytes < 0)
    {
        printf("Error receiving from Queue: %s\n", strerror(errno));
        return false;
    }
    buffer->text[nbytes] = '\0'; 
    return true;
}

bool PLUTO_MessageQueueWrite(PLUTO_MessageQueue_t queue, struct PLUTO_MsgBuf *buffer)
{
    assert(NULL != queue);
    bool return_value = true;
    buffer->msgtype = 1;
    size_t strl = strlen(buffer->text);
    int msgflags = IPC_NOWAIT;
    const int status = msgsnd(queue->filedescriptor, buffer, strl + 1, msgflags);
    if(status != 0)
    {
        printf(
            "Error writing to Queue %i, (errno: %i): %s - Data (size: %lu): %s\n", 
            queue->filedescriptor,
            errno,
            strerror(errno),
            strl,
            buffer->text
        );
        return_value = false;
    }
    return return_value;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
