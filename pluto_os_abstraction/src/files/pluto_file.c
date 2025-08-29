
#include <pluto/os_abstraction/pluto_time.h>
#include <pluto/os_abstraction/files/pluto_file.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>

#if PLUTO_KQUEUE_AVAILABLE
//
// kqueue
//
#include <sys/event.h>

#endif


struct PLUTO_File
{
    int descriptor;
    int kqueue;
    char *path;
};


PLUTO_File_t PLUTO_CreateFile(const char *filename)
{
    printf("Create File \"%s\".\n", filename);
    PLUTO_File_t file = (PLUTO_File_t)PLUTO_Malloc(sizeof(struct PLUTO_File));
    file->kqueue = -1;
    file->descriptor = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(file->descriptor >= 0)
    {
        file->path = PLUTO_Malloc(strlen(filename) + 1);
        memcpy(file->path, filename, strlen(filename));
        file->path[strlen(filename)] = '\0';
        return file;
    }
    PLUTO_Free(file);
    return NULL;
}

void PLUTO_DestroyFile(PLUTO_File_t *file)
{
    assert(NULL != *file);
    if((*file)->kqueue > 0)
    {
        (void)close((*file)->kqueue);
    }
    (void)close((*file)->descriptor);
    PLUTO_Free(*file);
    *file = NULL;
}

int PLUTO_FileGetDescriptor(const PLUTO_File_t file)
{
    return file->descriptor;
}

int32_t PLUTO_FileWrite(PLUTO_File_t file, const char *buffer, size_t nbytes)
{
    size_t res = 0;
    while(res != nbytes)
    {
        const int tmp = write(PLUTO_FileGetDescriptor(file), buffer, nbytes);
        if((tmp < 0) && (EAGAIN != errno))
        {
            return -1;
        }
        res += (size_t)tmp;
    }
    return 0;
}

int32_t PLUTO_FileRead(PLUTO_File_t file, char *buffer, size_t nbytes)
{
    return read(PLUTO_FileGetDescriptor(file), buffer, nbytes);
}

int32_t PLUTO_FileLockAquire(PLUTO_File_t file)
{
    (void)file;
    // https://linux.die.net/man/2/flock
    /*
    const int result = flock(file->descriptor, LOCK_EX | LOCK_NB);
    if(0 == result)
    {
        printf("Aquire Lock on File\n");
        return PLUTO_FILE_LOCK_AQUIRED;
    }
    printf("Error while aquirering a flock: %i, %s\n", result, strerror(errno));
    return PLUTO_FILE_LOCK_ALREADY_LOCKED;
    */
    return -1;
}

int32_t PLUTO_FileUnlockRelease(PLUTO_File_t file)
{
    (void)file;
    /*
    const int result = flock(file->descriptor, LOCK_UN);
    if(0 == result)
    {
        printf("Release Lock on File.\n");
        return PLUTO_FILE_LOCK_RELEASED;
    }
    printf("Release Lock on File Error: %i, %s.\n", result, strerror(errno));
    return PLUTO_FILE_LOCK_ERROR;
    */
    return -1;
}

