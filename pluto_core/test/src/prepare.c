#include <pluto/pluto_core/test/prepare.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

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


bool PLUTO_TEST_GenerateCoreConfig(const char *absolute_path_to_file)
{
    printf("Generate Config in %s\n", absolute_path_to_file);
    FILE *file = fopen(absolute_path_to_file, "w");
    if(!file)
    {
        return false;
    }
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
     */
    char content[4096];
    snprintf(
        content,
        sizeof(content),
        "{\n"
        "    \"nodes\":[\n"
        "        {\n"
        "            \"type\":\"python\",\n"
        "            \"name\":\"pluto-0\",\n"
        "            \"configuration-file\":\"%s\",\n"
        "            \"executable\":\"%s\"\n"
        "        }\n"
        "    ]\n"
        "}\n",
        PLUTO_TEST_config_path,
        PLUTO_TEST_executable
    );
    fwrite(content, strlen(content), 1, file);
    fclose(file);
    return true;
}

void PLUTO_TEST_RemoveGeneratedCoreConfig(const char *absolute_path_to_file)
{
    int result = remove(absolute_path_to_file);
    if(0 != result)
    {
        printf("Errno (%i): %s\n", errno, strerror(errno));
    }
}
