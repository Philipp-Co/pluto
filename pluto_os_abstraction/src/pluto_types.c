
//
// --------------------------------------------------------------------------------------------------------------------
//

#include "pluto/os_abstraction/pluto_logger.h"
#include <pluto/os_abstraction/pluto_types.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

static int PLUTO_TypesCreateFile(const char *path, const char *name, PLUTO_Key_t *key, PLUTO_Logger_t logger);

//
// --------------------------------------------------------------------------------------------------------------------
//
bool PLUTO_MkDir(const char *path, unsigned int permission, PLUTO_Logger_t logger)
{
    assert(NULL != path);
    assert('\0' == path[strlen(path)]); // Assert, that this is a C-Str.
    
    char *temp = PLUTO_Malloc(strlen(path) + 1);
    memcpy(temp, path, strlen(path) + 1);

    const size_t path_length = strlen(temp);
    if(path_length < 1)
    {
        PLUTO_LoggerWarning(logger, "Invalid Path, Path has 1 Byte only...");
        PLUTO_Free(temp);
        return false;
    }
    if('/' != temp[path_length - 1])
    {
        PLUTO_LoggerWarning(
            logger,
            "A Path should end with \"/\"."
        );
    }
    PLUTO_LoggerInfo(logger, "Mkdir for %s", path);
    for(char *pointer = strchr(temp + 1, '/'); pointer; pointer = strchr(pointer + 1, '/')) 
    {
        *pointer = '\0';
        PLUTO_LoggerInfo(
            logger,
            "Create Directory %s",
            temp
        );
        if(-1 == mkdir(temp, permission))
        {
            if(errno != EEXIST)
            {
                PLUTO_Free(temp);
                return false;
            }
        }
        *pointer = '/';
    }

    PLUTO_Free(temp);
    return true;
}

bool PLUTO_CreateKey(const char *path, const char *name, PLUTO_Key_t *key, PLUTO_Logger_t logger)
{
    //key->file = NULL;
    key->path_to_file = NULL;

    key->path_to_file = PLUTO_Malloc(strlen(name) + strlen(path) + 2);
    snprintf(key->path_to_file, strlen(path) + strlen(name) + 2,"%s%s", path, name);
    if(0 != PLUTO_TypesCreateFile(path, name, key, logger))
    {
        PLUTO_LoggerError(logger, "Error CreateKey...");
        PLUTO_DestroyKey(key);
        return false;
    }
    return true;
}

bool PLUTO_KeyGet(const char *path, const char *name, PLUTO_Key_t *key, PLUTO_Logger_t logger)
{
    assert(NULL != path);
    assert(NULL != name);
    assert(NULL != key);

    key->key = 0;
    //key->file = NULL;
    key->path_to_file = NULL;
    
    key->path_to_file = PLUTO_Malloc(strlen(name) + strlen(path) + 2);
    snprintf(key->path_to_file, strlen(path) + strlen(name) + 1,"%s%s", path, name);
    
    char buffer[4096]; 
    snprintf(buffer, sizeof(buffer), "%s%s", path, name);
    key->key = ftok(buffer, 1);
    if(-1 == key->key)
    {
        PLUTO_LoggerError(logger, "Unable to create key_t for \"%s%s\": %s\n", path, name, strerror(errno));
        key->key = 0;
        PLUTO_DestroyKey(key);

        assert(0 == key->key);
        //assert(NULL == key->file);
        assert(NULL == key->path_to_file);
        return false;
    }
    
    assert(0 != key->key);
    //assert(NULL != key->file);
    assert(NULL != key->path_to_file);
    return true;
}

void PLUTO_DestroyKey(PLUTO_Key_t *key)
{
    assert(NULL != key);

    if(NULL != key->path_to_file)
    {
        PLUTO_Free(key->path_to_file);
    }
    /*
    if(NULL != key->file)
    {
        fclose(key->file);
    }
    */
    key->path_to_file = NULL;
    //key->file = NULL;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
#include <sys/stat.h>

static int PLUTO_TypesCreateFile(const char *path, const char *name, PLUTO_Key_t *key, PLUTO_Logger_t logger)
{
    assert(NULL != path);
    assert(NULL != name);
    assert(NULL != key);

    int return_value = -1;

    if(!PLUTO_MkDir(path, 0777, logger))
    {
        PLUTO_LoggerError(
            logger,
            "Tying to create Key... Unable to create Directories for \"%s\", Error was \"%s\"", 
            path, 
            strerror(errno)
        );
        return_value = -1;
        goto end;
    }
    
    // Create a file
    char buffer[8192];
    snprintf(buffer, sizeof(buffer), "%s%s", path, name);
    FILE *fptr = fopen(buffer, "wb");
    if(fptr)
    {
        key_t ipc_key = ftok(buffer, 1);
        if(-1 == ipc_key)
        {
            PLUTO_LoggerError(logger, "Unable to create key_t: \"%s\"", strerror(errno));
            goto end;
        }
        key->key = ipc_key;
        //key->file = fptr;
        fclose(fptr);
    }
    else
    {
        PLUTO_LoggerError(logger, "Unable to create File \"%s%s\", Error was %s", path, name, strerror(errno));
    }
    return_value = 0;
end:
    return return_value;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
