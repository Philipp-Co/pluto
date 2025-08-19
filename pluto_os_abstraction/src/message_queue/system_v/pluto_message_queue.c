#include <pluto/os_abstraction/pluto_message_queue.h>

#ifdef PLUTO_MESSAGE_QUEUE_SYSTEM_V

///
/// \brief  System V Implementation of Message Queue.
///
//
// --------------------------------------------------------------------------------------------------------------------
//

#include "pluto/os_abstraction/pluto_logger.h"
#include <pluto/os_abstraction/pluto_semaphore.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <unistd.h> 
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

#define PLUTO_MQ_INPUT_QUEUE_PERMISSIONS 0777

//
// --------------------------------------------------------------------------------------------------------------------
//

PLUTO_MessageQueue_t PLUTO_CreateMessageQueue(
    const char *path, 
    const char *name, 
    unsigned int permissions, 
    PLUTO_Logger_t logger
)
{
    PLUTO_MessageQueue_t queue = (PLUTO_MessageQueue_t)PLUTO_Malloc(
        sizeof(struct PLUTO_MessageQueue)
    );
    if(!queue)
    {
        PLUTO_LoggerWarning(logger, "Unable to allocate Memory for MessageQueue.");
        return NULL;
    }

    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s-sem", name);
    queue->semaphore = PLUTO_CreateSemaphore(path, buffer, logger);
    if(!queue->semaphore)
    {
        PLUTO_LoggerWarning(logger, "Unable to create Semaphore.");
        goto error;
    }

    if(PLUTO_SEM_OK != PLUTO_SemaphoreSignal(queue->semaphore))
    {
        PLUTO_LoggerWarning(logger, "Unable to signal Semaphore.");
        goto error;
    }

    if(!PLUTO_CreateKey(path, name, &queue->key))
    {
        PLUTO_LoggerWarning(
            logger, 
            "Unable to create System V Key for Name: %s - %s", 
            name, 
            strerror(errno)
        );
        goto error;
    }

    PLUTO_LoggerInfo(logger, "Create Queue with Key 0x%x", queue->key.key);
    const int identifier = msgget(
        queue->key.key, 
        IPC_CREAT | IPC_NOWAIT | MSG_NOERROR | permissions
    );
    queue->filedescriptor = identifier;  
    if(identifier < 0)
    {
        PLUTO_LoggerWarning(
            logger, 
            "Error, unable to create Message Queue: %s...",
            strerror(errno)
        );
        goto error;
    }
    queue->logger = logger;
    return queue;
error:
    PLUTO_LoggerWarning(logger, "Unable to \"create\" MessageQueue abort with error.");
    PLUTO_DestroyMessageQueue(&queue);
    return NULL;
}

PLUTO_MessageQueue_t PLUTO_MessageQueueGet(const char *path, const char *name, PLUTO_Logger_t logger)
{
    PLUTO_MessageQueue_t queue = (PLUTO_MessageQueue_t)PLUTO_Malloc(
        sizeof(struct PLUTO_MessageQueue)
    );

    queue->filedescriptor = -1;
    queue->key.file = NULL;
    queue->key.key = 0;
    queue->key.path_to_file = NULL;
    queue->logger = NULL;
    queue->semaphore = NULL;
    
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s-sem", name);
    queue->semaphore = PLUTO_SemaphoreGet(path, buffer, logger);
    if(!queue->semaphore)
    {
        PLUTO_LoggerWarning(logger, "Unable to get Semaphore on Path %s with Name %s-sem", path, name);
        goto error;
    }
    
    if(PLUTO_SEM_OK != PLUTO_SemaphoreSignal(queue->semaphore))
    {
        PLUTO_LoggerWarning(logger, "Unable to signal Semaphore.");
        goto error;
    }
    
    if(!PLUTO_KeyGet(path, name, &queue->key))
    {
        PLUTO_LoggerWarning(logger, "Unable to get Key on Path %s with Name %s", path, name);
        goto error;
    }

    PLUTO_LoggerInfo(logger, "Get Queue with Key 0x%x", queue->key.key);
    queue->filedescriptor = msgget(
        queue->key.key, 0
    );
    if(queue->filedescriptor < 0)
    {
        PLUTO_LoggerWarning(logger, "Unable to open MessageQueue on Path %s with Name %s", path, name);
        goto error;
    }

    queue->logger = logger;
    return queue;
error:
    PLUTO_LoggerWarning(logger, "Unable to \"get\" MessageQueue abort with error.");
    PLUTO_DestroyMessageQueue(&queue);
    return NULL;
}


void PLUTO_DestroyMessageQueue(PLUTO_MessageQueue_t *queue)
{
    assert(NULL != *queue);
    if(*queue)
    {
        //
        // Decrement Reference Count.
        //
        if((*queue)->semaphore)
        {
            PLUTO_SemaphoreWait((*queue)->semaphore);
            const int32_t semaphore_value = PLUTO_SemaphoreValue((*queue)->semaphore);
            PLUTO_LoggerInfo(
                (*queue)->logger, 
                "Queue %s Ref. Count %i", 
                (*queue)->key.path_to_file != NULL ? (*queue)->key.path_to_file : "NULL", 
                semaphore_value
            );
            if(semaphore_value <= 0)
            {
                PLUTO_LoggerInfo(
                    (*queue)->logger,
                    "Destroy Queue %s", 
                    (*queue)->key.path_to_file != NULL ? (*queue)->key.path_to_file : "NULL" 
                );
                //
                // Only destroy Queue if the Reference Count indicates, 
                // that this is the last Instance which holds a Queue.
                //
                if((*queue)->filedescriptor >= 0)
                {
                    if(msgctl((*queue)->filedescriptor, IPC_RMID, NULL) < 0)
                    {
                        PLUTO_LoggerInfo(
                            (*queue)->logger, 
                            "Error, unable to delete MQ: %s",
                            strerror(errno)
                        );
                    }
                }
            }
            PLUTO_DestroySemaphore(&(*queue)->semaphore);
        }
        PLUTO_DestroyKey(&(*queue)->key);
        PLUTO_Free(*queue);
        *queue = NULL;
    }
}

bool PLUTO_MessageQueueRead(PLUTO_MessageQueue_t queue, struct PLUTO_MsgBuf *buffer)
{
    assert(NULL != queue);
    
    long msgtype = 0L;
    int msgflags = IPC_NOWAIT | MSG_NOERROR;
    buffer->msgtype = 1;
    const int nbytes = msgrcv(
        queue->filedescriptor, 
        buffer, 
        sizeof(buffer->text) - 1, 
        msgtype, 
        msgflags
    );
    if(nbytes < 0)
    {
        if(EAGAIN != errno && ENOMSG != errno)
        {
            PLUTO_LoggerWarning(
                queue->logger,
                "Error receiving from Queue: (%i) %s", 
                errno, 
                strerror(errno)
            );
        }
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
    size_t strl = sizeof(buffer->text) - 1;
    int msgflags = IPC_NOWAIT;
    int retry_count = 100;

    int status;
    do
    {
        status = msgsnd(queue->filedescriptor, buffer, strl + 1, msgflags);
        if((status < 0) && (errno != EAGAIN))
        {
            PLUTO_LoggerWarning(
                queue->logger,
                "Error writing to Queue %i, (errno: %i): %s - Data (size: %lu): %s", 
                queue->filedescriptor,
                errno,
                strerror(errno),
                strl,
                buffer->text
            );
            return_value = false;
            break;
        }
        else if((status < 0) && (EAGAIN == errno))
        {
            if(retry_count <= 0)
            {
                return_value = false;
                break;
            }
            retry_count--;
            usleep(100);
        }
        else if((status < 0) && (EINTR == errno))
        {
            return_value = false;
            break;
        }
        else
        {
            return_value = true;
            break;
        }
    } while(1);
    return return_value;
}

int32_t PLUTO_MessageQueueNumberOfMessagesAvailable(PLUTO_MessageQueue_t queue)
{
    struct msqid_ds data;
    msgctl(
        queue->filedescriptor,
        IPC_STAT,
        &data
    );
    return data.msg_qnum;
}
//
// --------------------------------------------------------------------------------------------------------------------
//

#endif
