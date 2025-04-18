#ifndef __PLUTO_MALLOC_H__
#define __PLUTO_MALLOC_H__

#include <stddef.h>


void* PLUTO_Malloc(size_t size);
void PLUTO_Free(void *ptr);

#if defined(PLUTO_TEST)
#include <stdbool.h>

void PLUTO_MallocResetState(void);
bool PLUTO_MallocCountEqual(void);
#endif

#endif
