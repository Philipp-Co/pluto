
//
// --------------------------------------------------------------------------------------------------------------------
//

#include "pluto/pluto_semaphore.h"
#include <pluto/pluto_message_queue.h>
#include <pluto/pluto_message_parser.h>

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

#define PLUTO_MAX_BODY_SIZE (1024 * 64)
#define PLUTO_MQ_INPUT_QUEUE_PERMISSIONS 0700 

//
// --------------------------------------------------------------------------------------------------------------------
//

struct PLUTO_MsgBuf 
{
    long msgtype;
    char text[PLUTO_MAX_BODY_SIZE];
};

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_MessageQueue_t PLUTO_CreateMessageQueue(const char *path, const char *name, unsigned int permissions)
{
    PLUTO_MessageQueue_t queue = (PLUTO_MessageQueue_t)malloc(
        sizeof(struct PLUTO_MessageQueue)
    );

    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s-sem", name);
    queue->semaphore = PLUTO_CreateSemaphore(path, buffer);
    if(!queue->semaphore)
    {
        free(queue);
        return NULL;
    }
    if(PLUTO_SEM_OK != PLUTO_SemaphoreSignal(queue->semaphore))
    {
        free(queue);
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
        free(queue);
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
        free(*queue);
        *queue = NULL;
    }
}

bool PLUTO_MessageQueueRead(PLUTO_MessageQueue_t queue, PLUTO_Request_t request)
{
    assert(NULL != queue);
    
    long msgtype = 0;
    size_t msgsize = PLUTO_MAX_BODY_SIZE-1;
    int msgflags = 0;
    struct PLUTO_MsgBuf *buffer = malloc(sizeof(struct PLUTO_MsgBuf));
    buffer->msgtype = 1;
    const int nbytes = msgrcv(queue->filedescriptor, buffer, msgsize, msgtype, msgflags);
    if(nbytes < 0)
    {
        printf("Error receiving from Queue: %s\n", strerror(errno));
        return false;
    }
    buffer->text[nbytes] = '\0'; 
    // parse Message...
    PLUTO_MessageParserLoadRequest(buffer->text, request);
    // Cleanup and return... 
    free(buffer);
    return true;
}

bool PLUTO_MessageQueueWrite(PLUTO_MessageQueue_t queue, PLUTO_Response_t response)
{
    assert(NULL != queue);
    bool return_value = true;
    // dump Message...
    struct PLUTO_MsgBuf *buffer = malloc(sizeof(struct PLUTO_MsgBuf));
    buffer->msgtype = 1;
    PLUTO_MessageParserDumpResponse(
        response,
        buffer->text,
        sizeof(buffer->text)
    );
    size_t strl = strlen(buffer->text);
    const size_t max_bytes = (strl >= (PLUTO_MAX_BODY_SIZE - 1)) ? (PLUTO_MAX_BODY_SIZE - 1) : strl;
    int msgflags = IPC_NOWAIT;
    const int status = msgsnd(queue->filedescriptor, buffer, max_bytes, msgflags);
    if(status != 0)
    {
        printf(
            "Error writing to Queue %i, (errno: %i): %s - Data (size: %lu): %s\n", 
            queue->filedescriptor,
            errno,
            strerror(errno),
            max_bytes,
            response->body
        );
        return_value = false;
    }
    free(buffer);
    return return_value;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
