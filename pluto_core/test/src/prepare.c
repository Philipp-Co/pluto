#include <pluto/pluto_core/test/prepare.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#ifndef PLUTO_CORE_TEST_CONFIG_PATH
#error "PLUTO_CORE_TEST_CONFIG_PATH is not set!"
#endif

#ifndef PLUTO_CORE_TEST_EXECUTABLE
#error "PLUTO_CORE_TEST_EXECUTABLE is not set!"
#endif

#ifndef PLUTO_CORE_TEST_BINARY_DIR
#error "PLUTO_CORE_TEST_BINARY_DIR is not set!"
#endif

const char* PLUTO_TEST_config_path = PLUTO_CORE_TEST_CONFIG_PATH;
const char* PLUTO_TEST_executable = PLUTO_CORE_TEST_EXECUTABLE;
const char* PLUTO_binary_dir = PLUTO_CORE_TEST_BINARY_DIR;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

static void PLUTO_TEST_MakeDirectory(const char *path)
{
    const int len = strlen(path);
    char buffer[1024];
    if(
        path[len-1] != '/'
    )
    {
        int i = len - 1;
        while(path[i] != '/')
        {
            --i;
        }
        memcpy(buffer, path, i);
    }
    else
    {
        memcpy(buffer, path, len);
    }
    mkdir(buffer, 0777);
}

bool PLUTO_TEST_WriteToConfigFile(const char *config_file_path, const char *content)
{
    PLUTO_TEST_MakeDirectory(config_file_path);
    FILE *file = fopen(config_file_path, "w+");
    if(!file)
    {
        printf("Error while creating File \"%s\"! Error was: %s\n", config_file_path, strerror(errno));
        return false;
    }
    if(1 != fwrite(content, strlen(content), 1, file))
    {
        printf("Error while writing Config File to Disk! Error was: %s\n", strerror(errno));
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

bool PLUTO_TEST_GenerateEmptyCoreConfig(const char *absolute_path_to_file)
{
    printf("Generate Config in %s\n", absolute_path_to_file);
    PLUTO_TEST_MakeDirectory(absolute_path_to_file);
    
    /*
     *
{
    "nodes":[
        {
            "type":"passthrough",
            "name":"pluto-0",
            "configuration-file":"/pluto-0-config.txt",
            "executable":""
        },
        {
            "type":"python",
            "name":"pluto-1",
            "configuration-file":"/pluto-1-config.txt",
            "executable":"pluto_event_handler"
        }
    ]
}
        "        {\n"
        "            \"type\":\"python\",\n"
        "            \"name\":\"pluto-0\",\n"
        "            \"configuration-file\":\"%s\",\n"
        "            \"executable\":\"%s\"\n"
        "        }\n"
     */
    char* content = 
        "{\n"
        "    \"nodes\":[]\n"
        "}\n";
    PLUTO_TEST_WriteToConfigFile(absolute_path_to_file, content);
    return true;
}

void PLUTO_TEST_RemoveGeneratedCoreConfig(const char *absolute_path_to_file)
{
    return;
    int result = remove(absolute_path_to_file);
    if(0 != result)
    {
        printf("Errno (%i): %s\n", errno, strerror(errno));
    }
}
