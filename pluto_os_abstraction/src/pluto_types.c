
//
// --------------------------------------------------------------------------------------------------------------------
//

#include <pluto/os_abstraction/pluto_types.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

//
// --------------------------------------------------------------------------------------------------------------------
//

static int PLUTO_TypesCreateFile(const char *path, const char *name, PLUTO_Key_t *key);

//
// --------------------------------------------------------------------------------------------------------------------
//

bool PLUTO_CreateKey(const char *path, const char *name, PLUTO_Key_t *key)
{
    key->file = NULL;
    key->path_to_file = NULL;

    key->path_to_file = malloc(strlen(name) + strlen(path) + 1);
    snprintf(key->path_to_file, strlen(path) + strlen(name) + 1,"%s%s", path, name);
    if(0 != PLUTO_TypesCreateFile(path, name, key))
    {
        PLUTO_DestroyKey(key);
        return false;
    }
    return true;
}

bool PLUTO_KeyGet(const char *path, const char *name, PLUTO_Key_t *key)
{
    key->key = 0;
    key->file = NULL;
    key->path_to_file = NULL;
    
    key->path_to_file = malloc(strlen(name) + strlen(path) + 1);
    snprintf(key->path_to_file, strlen(path) + strlen(name) + 1,"%s%s", path, name);
    
    char buffer[4096]; 
    snprintf(buffer, sizeof(buffer), "%s%s", path, name);
    key->key = ftok(buffer, 1);
    if(-1 == key->key)
    {
        printf("Unable to create key_t for %s%s: %s\n", path, name, strerror(errno));
        return false;
    }

    return true;
}

void PLUTO_DestroyKey(PLUTO_Key_t *key)
{
    if(key->path_to_file)
    {
        remove(key->path_to_file);
        free(key->path_to_file);
    }
    if(key->file)
    {
        fclose(key->file);
    }
    key->path_to_file = NULL;
    key->file = NULL;
}

//
// --------------------------------------------------------------------------------------------------------------------
//

static int PLUTO_TypesCreateFile(const char *path, const char *name, PLUTO_Key_t *key)
{
    int return_value = -1;
    FILE *fptr;

    char buffer[4096];
    snprintf(buffer, sizeof(buffer), "mkdir -m 0744 -p %s", path);
    const int mkdir_status = system(
        buffer
    );
    if(mkdir_status != 0)
    {
        fprintf(
            stderr, 
            "Tying to create Key..\nUnable to create Directories for %s, Error was \"%s\"\n", 
            path, 
            strerror(errno)
        );
        return_value = -1;
        goto end;
    }

    // Create a file
    snprintf(buffer, sizeof(buffer), "%s%s", path, name);
    fptr = fopen(buffer, "wb");
    if(fptr)
    {
        key_t ipc_key = ftok(buffer, 1);
        if(-1 == ipc_key)
        {
            printf("Unable to create key_t: %s\n", strerror(errno));
            goto end;
        }
        key->key = ipc_key;
        key->file = fptr;
    }
    else
    {
        fprintf(stderr, "Unable to create File %s%s, Error was %s\n", path, name, strerror(errno));
    }
    return_value = 0;
end:
    return return_value;
}

//
// --------------------------------------------------------------------------------------------------------------------
//
