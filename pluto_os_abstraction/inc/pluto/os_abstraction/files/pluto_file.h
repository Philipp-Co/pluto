#ifndef __PLUTO_FILES_H__
#define __PLUTO_FILES_H__

#include <pluto/os_abstraction/system_events/pluto_system_events.h>
#include <pluto/os_abstraction/pluto_time.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if PLUTO_CONFIG_ENABLE_INOTIFY
#include <sys/inotify.h>
#endif


struct PLUTO_File;
typedef struct PLUTO_File* PLUTO_File_t;


#if !defined(PLUTO_CONFIG_ENABLE_INOTIFY)
//
// If Inotify is not enable, declare standard Definitions.
//
#define PLUTO_FE_DATA_CHANGED 0x1
#define PLUTO_FE_FILE_DELETED 0x2

#endif

typedef struct
{
    PLUTO_Time_t timestamp;
    uint8_t event;
} PLUTO_FileEvent_t;

///
///
///
PLUTO_File_t PLUTO_CreateFile(const char *filename);
///
///
///
void PLUTO_DestroyFile(PLUTO_File_t *file);
///
///
///
int32_t PLUTO_FileWrite(PLUTO_File_t file, const char *buffer, size_t nbytes);
///
///
///
int32_t PLUTO_FileRead(PLUTO_File_t file, char *buffer, size_t nbytes);

#define PLUTO_FILE_LOCK_AQUIRED 0
#define PLUTO_FILE_LOCK_ALREADY_LOCKED -1
#define PLUTO_FILE_LOCK_RELEASED 0 
#define PLUTO_FILE_LOCK_ERROR -2

///
/// flock?
///
int32_t PLUTO_FileLockAquire(PLUTO_File_t file);
///
///
///
int32_t PLUTO_FileUnlockRelease(PLUTO_File_t file);

#define PLUTO_FE_OBS_OK 0
#define PLUTO_FE_OBS_ERROR -1

///
/// inotify?
///
int32_t PLUTO_FileRegisterObserver(PLUTO_File_t file, PLUTO_SystemEventHandler_t handler);

#endif
