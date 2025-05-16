#include <pluto/pluto_core/test/test_pluto_core_state.h>
#include <pluto/pluto_core/application_layer/pluto_core_state.h>
#include <pluto/pluto_config/pluto_config.h>

#include <stdlib.h>

void PLUTO_TEST_CoreStateInitial(void)
{
    char *binary_dir = getenv("PLUTO_BINARY_DIR");
    char *filename;
    PLUTO_CoreConfig_t config = PLUTO_CreateCoreConfig(
        filename,
        NULL
    );
    struct PLUTO_CoreState state = PLUTO_CreateCoreState(
        4LU,
        config,
        binary_dir,
        NULL
    ); 
    PLUTO_DestroyCoreState(&state);
}

