
#include <pluto/application_layer/pluto_processor.h>
#include <pluto/application_layer/shared_library/pluto_shared_library.h>

#include <dlfcn.h>
#include <stdio.h>


typedef PLUTO_ProcessorCallbackOutput_t (*PLUTO_SHLIB_ProcessEvent_t)(PLUTO_ProcessorCallbackInput_t *args);
typedef void (*PLUTO_SHLIB_Setup_t)(void);
typedef void (*PLUTO_SHLIB_Teardown_t)(void);

typedef struct
{
    PLUTO_SHLIB_Setup_t setup;
    PLUTO_SHLIB_Teardown_t teardown;
    PLUTO_SHLIB_ProcessEvent_t process;
} PLUTO_SHLIB_Library_t;

#define PLUTO_SHLIB_N_FUNCTIONS 3
static const char *PLUTO_SHLIB_expected_symbols[PLUTO_SHLIB_N_FUNCTIONS] = 
{
    "PLUTO_SharedLibrarySetupObject",
    "PLUTO_SharedLibraryTearDownObject",
    "PLUTO_SharedLibraryProcessEvent"    
};
static void* PLUTO_SHLIB_shared_lirary_handle = NULL;
static PLUTO_SHLIB_Library_t PLUTO_SHLIB_library = 
{
    .setup = NULL,
    .teardown = NULL,
    .process = NULL
};

bool PLUTO_SHLIB_Initialize(const char *path)
{
    PLUTO_SHLIB_shared_lirary_handle = dlopen(path, RTLD_NOW);
    if(!PLUTO_SHLIB_shared_lirary_handle)
    {
        printf("Error loading shared Library: %s\n", dlerror());
        return false;
    }
    
    PLUTO_SHLIB_library.setup = (PLUTO_SHLIB_Setup_t)dlsym(PLUTO_SHLIB_shared_lirary_handle, PLUTO_SHLIB_expected_symbols[0]);
    if(!PLUTO_SHLIB_library.setup)
    {
        printf("Error the given shared Library does not Contain the Function: \"%s\"\n", PLUTO_SHLIB_expected_symbols[0]);
        PLUTO_SHLIB_Destroy();
        return false;
    }
    PLUTO_SHLIB_library.teardown = (PLUTO_SHLIB_Teardown_t)dlsym(PLUTO_SHLIB_shared_lirary_handle, PLUTO_SHLIB_expected_symbols[1]);
    if(!PLUTO_SHLIB_library.teardown)
    {
        printf("Error the given shared Library does not Contain the Function: \"%s\"\n", PLUTO_SHLIB_expected_symbols[1]);
        PLUTO_SHLIB_Destroy();
        return false;
    }
    PLUTO_SHLIB_library.process = (PLUTO_SHLIB_ProcessEvent_t)dlsym(PLUTO_SHLIB_shared_lirary_handle, PLUTO_SHLIB_expected_symbols[2]);
    if(!PLUTO_SHLIB_library.process)
    {
        printf("Error the given shared Library does not Contain the Function: \"%s\"\n", PLUTO_SHLIB_expected_symbols[2]);
        PLUTO_SHLIB_Destroy();
        return false;
    }

    PLUTO_SHLIB_library.setup();
    return true;
}

void PLUTO_SHLIB_Destroy(void)
{
    if(PLUTO_SHLIB_shared_lirary_handle)
    {
        if(PLUTO_SHLIB_library.teardown)
        {
            PLUTO_SHLIB_library.teardown();
        }
        dlclose(PLUTO_SHLIB_shared_lirary_handle);
        PLUTO_SHLIB_shared_lirary_handle = NULL;
    }
}

PLUTO_ProcessorCallbackOutput_t PLUTO_SHLIB_ProcessCallback(PLUTO_ProcessorCallbackInput_t *args)
{
    return PLUTO_SHLIB_library.process(args);
}
