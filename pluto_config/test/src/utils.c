#include <utils.h>
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
