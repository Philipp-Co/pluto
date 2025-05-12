#ifndef __PLUTO_CONFIG_PRIVATE_FILE_H__
#define __PLUTO_CONFIG_PRIVATE_FILE_H__

#include <pluto/os_abstraction/pluto_logger.h>

#include <stddef.h>


struct PLUTO_CONFIG_Buffer
{
    void *buffer;
    size_t nbytes;
};
typedef struct PLUTO_CONFIG_Buffer* PLUTO_CONFIG_Buffer_t;

PLUTO_CONFIG_Buffer_t PLUTO_CONFIG_CreateBuffer(size_t n_bytes);
void PLUTO_CONFIG_DestroyBuffer(PLUTO_CONFIG_Buffer_t *buffer);

PLUTO_CONFIG_Buffer_t PLUTO_CONFIG_ReadFile(const char *filename, PLUTO_Logger_t logger);

#endif
