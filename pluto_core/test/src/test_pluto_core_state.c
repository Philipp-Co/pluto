#include <Unity/src/unity.h>
#include <pluto/pluto_core/test/prepare.h>
#include <pluto/pluto_core/test/test_pluto_core_state.h>
#include <pluto/pluto_core/application_layer/pluto_core_state.h>
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/pluto_core/data_layer/pluto_core_register.h>

#include <stdlib.h>


void PLUTO_TEST_CoreStateInitial(void)
{
    PLUTO_TEST_GenerateEmptyCoreConfig(PLUTO_TEST_config_path);
    
    PLUTO_Logger_t logger = PLUTO_CreateLogger("test-CoreState");

    PLUTO_CoreConfig_t config = PLUTO_CreateCoreConfig(
        PLUTO_TEST_config_path,
        logger
    );
    if(!config)
    {
        goto error;
    }
    
    PLUTO_CoreRegister_t core_register = PLUTO_CreateCoreRegister(logger);

    struct PLUTO_CoreState state = PLUTO_CreateCoreState(
        PLUTO_CoreConfigNumberOfNodes(config),
        config,
        PLUTO_CORE_TEST_BINARY_DIR,
        core_register,
        logger
    ); 

    PLUTO_CoreStateEvent_t event = {0};
    PLUTO_CoreStateEventForProcess(&event);
    PLUTO_CoreStateDispatchEvent(&state, &event);

    PLUTO_DestroyCoreState(&state);
    PLUTO_DestroyCoreConfig(&config);
    PLUTO_DestroyCoreRegister(&core_register);
    PLUTO_DestroyLogger(&logger);
    
    PLUTO_TEST_RemoveGeneratedCoreConfig(PLUTO_TEST_config_path);
    return;
error:
    //
    // Something went wrong...
    //
    PLUTO_DestroyLogger(&logger);
    PLUTO_TEST_RemoveGeneratedCoreConfig(PLUTO_TEST_config_path);
    TEST_ASSERT_TRUE(false);
}

