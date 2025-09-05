#include "pluto/os_abstraction/pluto_types.h"
#include <pluto/os_abstraction/message_queue/pluto_message_queue.h>

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
#include <time.h>
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
    queue->internal = PLUTO_Malloc(sizeof(struct PLUTO_MessageQueueInternal));

    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s-sem", name);
    queue->internal->semaphore = PLUTO_CreateSemaphore(path, buffer, logger);
    if(!queue->internal->semaphore)
    {
        PLUTO_LoggerWarning(logger, "Unable to create Semaphore.");
        goto error;
    }

    if(PLUTO_SEM_OK != PLUTO_SemaphoreSignal(queue->internal->semaphore))
    {
        PLUTO_LoggerWarning(logger, "Unable to signal Semaphore.");
        goto error;
    }

    queue->internal->key = PLUTO_Malloc(sizeof(PLUTO_Key_t));
    if(!PLUTO_CreateKey(path, name, queue->internal->key))
    {
        PLUTO_LoggerWarning(
            logger,
            "Unable to create System V Key for Name: %s - %s",
            name,
            strerror(errno)
        );
        goto error;
    }

    PLUTO_LoggerInfo(logger, "Create Queue with Key 0x%x", queue->internal->key->key);
    const int identifier = msgget(
        queue->internal->key->key,
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
    queue->internal->logger = logger;
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
    queue->internal = PLUTO_Malloc(sizeof(struct PLUTO_MessageQueueInternal));

    queue->filedescriptor = -1;
    queue->internal->logger = NULL;
    queue->internal->semaphore = NULL;

    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s-sem", name);
    queue->internal->semaphore = PLUTO_SemaphoreGet(path, buffer, logger);
    if(!queue->internal->semaphore)
    {
        PLUTO_LoggerWarning(logger, "Unable to get Semaphore on Path %s with Name %s-sem", path, name);
        goto error;
    }

    if(PLUTO_SEM_OK != PLUTO_SemaphoreSignal(queue->internal->semaphore))
    {
        PLUTO_LoggerWarning(logger, "Unable to signal Semaphore.");
        goto error;
    }

    queue->internal->key = PLUTO_Malloc(sizeof(PLUTO_Key_t));
    queue->internal->key->file = NULL;
    queue->internal->key->key = 0;
    queue->internal->key->path_to_file = NULL;
    if(!PLUTO_KeyGet(path, name, queue->internal->key))
    {
        PLUTO_LoggerWarning(logger, "Unable to get Key on Path %s with Name %s", path, name);
        goto error;
    }

    PLUTO_LoggerInfo(logger, "Get Queue with Key 0x%x", queue->internal->key->key);
    queue->filedescriptor = msgget(
        queue->internal->key->key, 0
    );
    if(queue->filedescriptor < 0)
    {
        PLUTO_LoggerWarning(logger, "Unable to open MessageQueue on Path %s with Name %s", path, name);
        goto error;
    }

    queue->internal->logger = logger;

    const int32_t semaphore_value = PLUTO_SemaphoreValue(queue->internal->semaphore);
    PLUTO_LoggerInfo(
        logger,
        "Queue %s Ref. Count %i",
        queue->internal->key->path_to_file != NULL ? queue->internal->key->path_to_file : "NULL",
        semaphore_value
    );
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
        if((*queue)->internal->semaphore)
        {
            const PLUTO_SEM_ReturnValue_t result = PLUTO_SemaphoreWait((*queue)->internal->semaphore);
            PLUTO_LoggerInfo(
                (*queue)->internal->logger,
                "Sem Wait Result: %i",
                result
            );
            const int32_t semaphore_value = PLUTO_SemaphoreValue((*queue)->internal->semaphore);
            PLUTO_LoggerInfo(
                (*queue)->internal->logger,
                "Queue %s Ref. Count %i",
                (*queue)->internal->key->path_to_file != NULL ? (*queue)->internal->key->path_to_file : "NULL",
                semaphore_value
            );
            if(semaphore_value <= 0)
            {
                PLUTO_LoggerInfo(
                    (*queue)->internal->logger,
                    "Destroy Queue %s",
                    (*queue)->internal->key->path_to_file != NULL ? (*queue)->internal->key->path_to_file : "NULL"
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
                            (*queue)->internal->logger,
                            "Error, unable to delete MQ: %s",
                            strerror(errno)
                        );
                    }
                }
            }
            PLUTO_DestroySemaphore(&(*queue)->internal->semaphore);
        }
        PLUTO_DestroyKey((*queue)->internal->key);
        PLUTO_Free((*queue)->internal->key);
        PLUTO_Free((*queue)->internal);
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
                queue->internal->logger,
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
                queue->internal->logger,
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
            struct timespec ts;
            int res;
            const long msec = 1000;
            if (msec < 0)
            {
                errno = EINVAL;
                return -1;
            }

            ts.tv_sec = msec / 1000;
            ts.tv_nsec = (msec % 1000) * 1000000;

            do {
                res = nanosleep(&ts, &ts);
            } while (res && errno == EINTR);
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
