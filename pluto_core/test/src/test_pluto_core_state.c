#include <Unity/src/unity.h>
#include <pluto/pluto_core/test/prepare.h>
#include <pluto/pluto_core/test/test_pluto_core_state.h>
#include <pluto/pluto_core/application_layer/pluto_core_state.h>
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_logger.h>

#include <stdlib.h>


void PLUTO_TEST_CoreStateInitial(void)
{
    PLUTO_Logger_t logger = PLUTO_CreateLogger("test-CoreState");

    char *binary_dir = getenv("PLUTO_BINARY_DIR");
    PLUTO_CoreConfig_t config = PLUTO_CreateCoreConfig(
        PLUTO_TEST_config_path,
        logger
    );
    if(!config)
    {
        goto error;
    }
    struct PLUTO_CoreState state = PLUTO_CreateCoreState(
        PLUTO_CoreConfigNumberOfNodes(config),
        config,
        binary_dir,
        logger
    ); 

    PLUTO_CoreStateEvent_t event = {0};
    PLUTO_CoreStateEventForProcess(&event);
    PLUTO_CoreStateDispatchEvent(&state, &event);

    PLUTO_DestroyCoreState(&state);
    PLUTO_DestroyCoreConfig(&config);
    PLUTO_DestroyLogger(&logger);
    return;
error:
    //
    // Something went wrong...
    //
    PLUTO_DestroyLogger(&logger);
    TEST_ASSERT_TRUE(false);
}

