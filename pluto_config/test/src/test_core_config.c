#include <utils.h>
#include <test_pluto_config.h>
#include <test_core_config.h>
#include <pluto/pluto_config/pluto_config.h>

#include <stddef.h>
#include <stdio.h>

#include <Unity/src/unity.h>

extern PLUTO_Logger_t TEST_PLUTO_Logger;

static bool PLUTO_TEST_CreateEmptyCoreConfig(void);
static bool PLUTO_TEST_CreateMultipleNodesCoreConfig(void);
static void PLUTO_TEST_CoreConfigFile(const char* config_file_name, char *buffer, size_t size);

void test_core_initial(void)
{
    TEST_ASSERT_TRUE(PLUTO_TEST_CreateEmptyCoreConfig());
    char config_file_path[512];
    PLUTO_TEST_CoreConfigFile("core/empty_core_config.txt", config_file_path, sizeof(config_file_path));

    PLUTO_CoreConfig_t config = PLUTO_CreateCoreConfig(
        config_file_path,
        TEST_PLUTO_Logger
    );

    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_EQUAL_size_t(
        0,
        PLUTO_CoreConfigNumberOfNodes(config)
    );

    PLUTO_DestroyCoreConfig(&config);
    TEST_ASSERT_NULL(config);
}

void test_core_multiple_nodes(void)
{
    TEST_ASSERT_TRUE(PLUTO_TEST_CreateMultipleNodesCoreConfig());
    char config_file_path[512];
    PLUTO_TEST_CoreConfigFile("core/multiple_nodes_config.txt", config_file_path, sizeof(config_file_path));

    PLUTO_CoreConfig_t config = PLUTO_CreateCoreConfig(
        config_file_path,
        TEST_PLUTO_Logger
    );

    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_EQUAL_size_t(
        3,
        PLUTO_CoreConfigNumberOfNodes(config)
    ); 

    PLUTO_Config_t node_config = NULL;
    for(size_t i=0;i<PLUTO_CoreConfigNumberOfNodes(config);++i)
    {
        node_config = PLUTO_CoreConfigGetNodeConfig(config, (int32_t)i);
        
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "input_%i", (int32_t)i);
        TEST_ASSERT_EQUAL_STRING(
            buffer,
            PLUTO_ConfigNameOfInputQueue(node_config)
        );
        TEST_ASSERT_EQUAL_INT32(
            1,
            PLUTO_ConfigNumberOfOutputQueues(node_config)
        );
        snprintf(buffer, sizeof(buffer), "output_%i", (int32_t)i);
        TEST_ASSERT_EQUAL_STRING(
            buffer,
            PLUTO_ConfigNameOfOutputQueue(node_config, 0)
        );
    }

    PLUTO_DestroyCoreConfig(&config);
    TEST_ASSERT_NULL(config);
}

static void PLUTO_TEST_CoreConfigFile(const char* config_file_name, char *buffer, size_t size)
{
    snprintf(
        buffer,
        size,
        "%s%s",
        TEST_PLUTO_CONFIG_RESSOURCE_DIR,
        config_file_name
    );
}

//
// ------------------------------------------------------------------------------------------------
//

static bool PLUTO_TEST_CreateEmptyCoreConfig(void)
{
    char filename[1024];
    snprintf(
        filename,
        sizeof(filename),
        "%score/empty_core_config.txt",
        TEST_PLUTO_CONFIG_RESSOURCE_DIR
    );
    char *content = "{\"nodes\":[]}";
    return PLUTO_TEST_WriteToConfigFile(
        filename,
        content
    ); 
}

static bool PLUTO_TEST_CreateMultipleNodesCoreConfig(void)
{
    char core_config_file[2048];
    snprintf(
        core_config_file,
        sizeof(core_config_file),
        "%score/multiple_nodes_config.txt",
        TEST_PLUTO_CONFIG_RESSOURCE_DIR
    );

    char content[4096]; 
    snprintf(
        content,
        sizeof(content),
        "{"
          "\"nodes\": ["
            "{"
              "\"name\": \"node0\","
              "\"type\": \"python\","
              "\"configuration-file\": \"%score/node0.txt\","
              "\"executable\": \"plyto._internal.plyto_default_handler\""
            "},"
            "{"
              "\"name\": \"node1\","
              "\"type\": \"passthrough\","
              "\"configuration-file\": \"%score/node1.txt\","
              "\"executable\": \"test2.py\""
            "},"
            "{"
              "\"name\": \"node2\","
              "\"type\": \"passthrough\","
              "\"configuration-file\": \"%score/node2.txt\","
              "\"executable\": \"test3.py\""
            "}"
          "]"
        "}",
        TEST_PLUTO_CONFIG_RESSOURCE_DIR,
        TEST_PLUTO_CONFIG_RESSOURCE_DIR,
        TEST_PLUTO_CONFIG_RESSOURCE_DIR
    );
    if(!PLUTO_TEST_WriteToConfigFile(core_config_file, content))
    {
        return false;
    }
    char node_config_file[2048];
    char node_config[1024];
    for(int i=0;i<3;++i)
    {
        snprintf(
            node_config_file,
            sizeof(node_config_file),
            "%score/node%i.txt",
            TEST_PLUTO_CONFIG_RESSOURCE_DIR,
            i
        );
        snprintf(
            node_config,
            sizeof(node_config),
            "{"
              "\"work_dir\": \"%s\","
              "\"name_of_input_queue\": \"input_%i\","
              "\"names_of_output_queues\": [\"output_%i\"]"
            "}",
            TEST_PLUTO_WORKDIR,
            i,
            i
        );
        if(!PLUTO_TEST_WriteToConfigFile(node_config_file, node_config))
        {
            return false;
        }
    }
    return true;
}
