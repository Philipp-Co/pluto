#include <pluto/pluto_config/private/pluto_file.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>


PLUTO_CONFIG_Buffer_t PLUTO_CONFIG_CreateBuffer(size_t n_bytes)
{
    PLUTO_CONFIG_Buffer_t buffer = PLUTO_Malloc(sizeof(struct PLUTO_CONFIG_Buffer));

    buffer->buffer = PLUTO_Malloc(n_bytes);
    buffer->nbytes = n_bytes; 
   
    return buffer; 
}

void PLUTO_CONFIG_DestroyBuffer(PLUTO_CONFIG_Buffer_t *buffer)
{
    PLUTO_Free((*buffer)->buffer);
    PLUTO_Free(*buffer);
    *buffer = NULL;
}

PLUTO_CONFIG_Buffer_t PLUTO_CONFIG_ReadFile(const char *filename, PLUTO_Logger_t logger)
{
    PLUTO_CONFIG_Buffer_t buffer = NULL;   
    PLUTO_LoggerInfo(logger, "Read Config File: %s", filename);
    FILE *file = fopen(filename, "r");
    if(!file)
    {
        PLUTO_LoggerError(logger, "Unable to open Config File.\n");
        return NULL;
    }
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    if(size <= 0)
    {
        PLUTO_LoggerError(logger, "File has size 0.\n");
        fclose(file);
        return NULL;
    }
    fseek(file, 0L, SEEK_SET);
    buffer = PLUTO_CONFIG_CreateBuffer(size + 1);
    memset(buffer->buffer, '\0', buffer->nbytes);

    size_t result = fread(buffer->buffer, 1, buffer->nbytes - 1, file);
    if(result == 0)
    {
        PLUTO_LoggerError(logger, "Result was 0.\n");
        PLUTO_CONFIG_DestroyBuffer(&buffer);
        buffer = NULL;
    }
    fclose(file);
    return buffer;
}
