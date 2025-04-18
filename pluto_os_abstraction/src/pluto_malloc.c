#include <pluto/os_abstraction/pluto_malloc.h>

#include <stdlib.h>


#if defined(PLUTO_TEST)
typedef struct
{
    long n_mallocs;
    long n_frees;
} PLUTO_Malloc_t;

static PLUTO_Malloc_t PLUTO_mstate = {0};
#endif

void* PLUTO_Malloc(size_t size)
{
#if defined(PLUTO_TEST)
    PLUTO_mstate.n_mallocs++;
#endif
    return malloc(size);
}

void PLUTO_Free(void *ptr)
{
#if defined(PLUTO_TEST)
    PLUTO_mstate.n_frees++;
#endif
    free(ptr);
}

#if defined(PLUTO_TEST)
void PLUTO_MallocResetState(void)
{
    PLUTO_mstate.n_mallocs = 0;
    PLUTO_mstate.n_frees = 0;
}

bool PLUTO_MallocCountEqual(void)
{
    return PLUTO_mstate.n_mallocs == PLUTO_mstate.n_frees;
}
#endif
