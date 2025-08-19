#include <pluto/os_abstraction/pluto_malloc.h>

#include <stdlib.h>
#include <stdio.h>

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

    void *mem = malloc(size);
#if defined(PLUTO_TEST)
    //printf("Malloc: %p\n", mem);
#endif
    return mem;
}

void PLUTO_Free(void *ptr)
{
#if defined(PLUTO_TEST)
    PLUTO_mstate.n_frees++;
    //printf("Free: %p\n", ptr);
#endif
    free(ptr);
}

#if defined(PLUTO_TEST)
#include <stdio.h>
void PLUTO_MallocResetState(void)
{
    PLUTO_mstate.n_mallocs = 0;
    PLUTO_mstate.n_frees = 0;
}

bool PLUTO_MallocCountEqual(void)
{
    if(PLUTO_mstate.n_mallocs != PLUTO_mstate.n_frees)
    {
        printf("Mallocs: %lu, Frees: %lu\n", PLUTO_mstate.n_mallocs, PLUTO_mstate.n_frees);
    }
    return PLUTO_mstate.n_mallocs == PLUTO_mstate.n_frees;
}
#endif
