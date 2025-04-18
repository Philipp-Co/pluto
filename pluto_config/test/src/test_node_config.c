#include "Unity/src/unity.h"
#include <pluto/pluto_config/pluto_config.h>
#include <test_pluto_config.h>
#include <test_node_config.h>
#include <utils.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

extern PLUTO_Logger_t TEST_PLUTO_Logger;

static bool PLUTO_TEST_CreateEmptyNodeConfig(void);
static bool PLUTO_TEST_CreateMinimalNodeConfig(void);
static bool PLUTO_TEST_CreateMultipleOutputQueueNodeConfig(void);
static bool PLUTO_TEST_CreatePythonNodeConfig(void);


void test_initial(void)
{
    TEST_ASSERT_TRUE(PLUTO_TEST_CreateEmptyNodeConfig());
    char *ressource_dir = TEST_PLUTO_CONFIG_RESSOURCE_DIR;
    TEST_ASSERT_NOT_NULL(ressource_dir);
    
    char filename[512];
    snprintf(
        filename, sizeof(filename), "%snode/empty_node_config.txt", ressource_dir
    );
    const char *name = "test_node_0\0";

    PLUTO_Config_t config = PLUTO_CreateConfig(
        filename, name, TEST_PLUTO_Logger
    );
    
    TEST_ASSERT_EQUAL_STRING(
        "",
        PLUTO_ConfigNameOfInputQueue(config)
    );
    TEST_ASSERT_EQUAL_INT32(
        0,
        PLUTO_ConfigNumberOfOutputQueues(config)
    );
    //
    // Code ASserts Index Access!
    //
    //TEST_ASSERT_NULL(
    //    PLUTO_ConfigNameOfOutputQueue(config, 0)
    //);
    TEST_ASSERT_FALSE(
        PLUTO_ConfigIsPythonPathSet(config)
    );
    TEST_ASSERT_EQUAL_STRING(
        "",
        PLUTO_ConfigPythonPath(config)
    );
    
    PLUTO_DestroyConfig(
        &config
    );
    TEST_ASSERT_NULL(config);
}

void test_minimal_config(void)
{
    TEST_ASSERT_TRUE(PLUTO_TEST_CreateMinimalNodeConfig());
    char *ressource_dir = TEST_PLUTO_CONFIG_RESSOURCE_DIR;
    TEST_ASSERT_NOT_NULL(ressource_dir);
    
    char filename[512];
    snprintf(
        filename, sizeof(filename), "%snode/minimal_config.txt", ressource_dir
    );
    const char *name = "test_node_0\0";

    PLUTO_Config_t config = PLUTO_CreateConfig(
        filename, name, TEST_PLUTO_Logger
    );
   
    TEST_ASSERT_EQUAL_STRING(
        "test_input_queue",
        PLUTO_ConfigNameOfInputQueue(config)
    );
    TEST_ASSERT_EQUAL_INT32(
        1,
        PLUTO_ConfigNumberOfOutputQueues(config)
    );
    TEST_ASSERT_EQUAL_STRING(
        "test_output_queue",
        PLUTO_ConfigNameOfOutputQueue(config, 0)
    );
    TEST_ASSERT_FALSE(
        PLUTO_ConfigIsPythonPathSet(config)
    );
    TEST_ASSERT_EQUAL_STRING(
        "",
        PLUTO_ConfigPythonPath(config)
    );
        

    PLUTO_DestroyConfig(
        &config
    );
    TEST_ASSERT_NULL(config);
}

void test_multiple_output_queues_config(void)
{
    TEST_ASSERT_TRUE(PLUTO_TEST_CreateMultipleOutputQueueNodeConfig());
    char *ressource_dir = TEST_PLUTO_CONFIG_RESSOURCE_DIR;
    TEST_ASSERT_NOT_NULL(ressource_dir);
    
    char filename[512];
    snprintf(
        filename, sizeof(filename), "%snode/multiple_output_queues_config.txt", ressource_dir
    );
    const char *name = "test_node_0\0";

    PLUTO_Config_t config = PLUTO_CreateConfig(
        filename, name, TEST_PLUTO_Logger
    );
   
    TEST_ASSERT_EQUAL_STRING(
        "test_input_queue",
        PLUTO_ConfigNameOfInputQueue(config)
    );
    TEST_ASSERT_EQUAL_INT32(
        3,
        PLUTO_ConfigNumberOfOutputQueues(config)
    );
    TEST_ASSERT_EQUAL_STRING(
        "test_output_queue_0",
        PLUTO_ConfigNameOfOutputQueue(config, 0)
    );
    TEST_ASSERT_EQUAL_STRING(
        "test_output_queue_1",
        PLUTO_ConfigNameOfOutputQueue(config, 1)
    );
    TEST_ASSERT_EQUAL_STRING(
        "test_output_queue_2",
        PLUTO_ConfigNameOfOutputQueue(config, 2)
    );
    TEST_ASSERT_FALSE(
        PLUTO_ConfigIsPythonPathSet(config)
    );
    TEST_ASSERT_EQUAL_STRING(
        "",
        PLUTO_ConfigPythonPath(config)
    );
        
    PLUTO_DestroyConfig(
        &config
    );
    TEST_ASSERT_NULL(config);
}

void test_python_config(void)
{
    TEST_ASSERT_TRUE(PLUTO_TEST_CreatePythonNodeConfig());
    char *ressource_dir = TEST_PLUTO_CONFIG_RESSOURCE_DIR;
    TEST_ASSERT_NOT_NULL(ressource_dir);
    
    char filename[512];
    snprintf(
        filename, sizeof(filename), "%snode/python_config.txt", ressource_dir
    );
    const char *name = "test_node_0\0";

    PLUTO_Config_t config = PLUTO_CreateConfig(
        filename, name, TEST_PLUTO_Logger
    );
   
    TEST_ASSERT_EQUAL_STRING(
        "test_input_queue",
        PLUTO_ConfigNameOfInputQueue(config)
    );
    TEST_ASSERT_EQUAL_INT32(
        0,
        PLUTO_ConfigNumberOfOutputQueues(config)
    );
    TEST_ASSERT_TRUE(
        PLUTO_ConfigIsPythonPathSet(config)
    );
    TEST_ASSERT_EQUAL_STRING(
        "path_0;path_1",
        PLUTO_ConfigPythonPath(config)
    );
        
    PLUTO_DestroyConfig(
        &config
    );
    TEST_ASSERT_NULL(config);
}

//
// ------------------------------------------------------------------------------------------------
//

static bool PLUTO_TEST_CreateEmptyNodeConfig(void)
{
    char *ressource_dir = TEST_PLUTO_CONFIG_RESSOURCE_DIR;
    char filename[1024];
    snprintf(
        filename, sizeof(filename), "%snode/empty_node_config.txt", ressource_dir
    );
    const char *content = "{"
      "\"work_dir\": \"\","
      "\"name_of_input_queue\": \"\","
      "\"names_of_output_queues\": []"
    "}";
    return PLUTO_TEST_WriteToConfigFile(
        filename,
        content
    );
}

static bool PLUTO_TEST_CreateMinimalNodeConfig(void)
{
    char *ressource_dir = TEST_PLUTO_CONFIG_RESSOURCE_DIR;
    char filename[512];
    snprintf(
        filename, sizeof(filename), "%snode/minimal_config.txt", ressource_dir
    );
    char content[1024];
    snprintf(
        content, 
        sizeof(content),
        "{"
          "\"work_dir\": \"%s\","
          "\"name_of_input_queue\": \"test_input_queue\","
          "\"names_of_output_queues\": [\"test_output_queue\"]"
        "}",
        TEST_PLUTO_WORKDIR
    );
    return PLUTO_TEST_WriteToConfigFile(
        filename,
        content
    );
}

static bool PLUTO_TEST_CreateMultipleOutputQueueNodeConfig(void)
{
    char *ressource_dir = TEST_PLUTO_CONFIG_RESSOURCE_DIR;
    char filename[512];
    snprintf(
        filename, sizeof(filename), "%snode/multiple_output_queues_config.txt", ressource_dir
    );
    char content[1024]; 
    snprintf(
      content,
      sizeof(content),
      "{"
          "\"work_dir\": \"%s\","
          "\"name_of_input_queue\": \"test_input_queue\","
          "\"names_of_output_queues\": ["
            "\"test_output_queue_0\","
            "\"test_output_queue_1\","
            "\"test_output_queue_2\""
            "]"
        "}",
        TEST_PLUTO_WORKDIR
     );

    return PLUTO_TEST_WriteToConfigFile(
        filename,
        content
    );
}

static bool PLUTO_TEST_CreatePythonNodeConfig(void)
{
    char *ressource_dir = TEST_PLUTO_CONFIG_RESSOURCE_DIR;
    char filename[512];
    snprintf(
        filename, sizeof(filename), "%snode/python_config.txt", ressource_dir
    );
    char content[1024];
    snprintf(
        content,
        sizeof(content),
        "{"
          "\"work_dir\": \"%s\","
          "\"name_of_input_queue\": \"test_input_queue\","
          "\"names_of_output_queues\": [],"
          "\"python_path\": \"path_0;path_1\""
        "}",
        TEST_PLUTO_WORKDIR
    );

    return PLUTO_TEST_WriteToConfigFile(
        filename,
        content
    );
}
