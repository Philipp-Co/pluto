
#include "pluto/os_abstraction/pluto_shared_memory.h"
#include <pluto/pluto_core/data_layer/pluto_core_register.h>
#include <pluto/os_abstraction/pluto_malloc.h>

#include <string.h>
#include <assert.h>
#include <stdio.h>


static struct PLUTO_CoreInfo* PLUTO_CoreRegisterGetInfo(PLUTO_CoreRegister_t reg)
{
    return (struct PLUTO_CoreInfo*)PLUTO_SharedMemoryAddress(reg->shm);
}

PLUTO_CoreRegister_t PLUTO_CreateCoreRegister(PLUTO_Logger_t logger)
{
    const char *path = "/tmp/pluto/";
    const char *name = "core.info";
    const size_t nbytes = sizeof(struct PLUTO_CoreInfo);
    PLUTO_SharedMemory_t shm = PLUTO_CreateSharedMemory(nbytes, path, name, logger);

    PLUTO_CoreRegister_t reg = (PLUTO_CoreRegister_t)PLUTO_Malloc(sizeof(struct PLUTO_CoreRegister));
    reg->shm = shm;
    PLUTO_CoreRegisterGetInfo(reg)->nuber_of_nodes = 0;
    return reg;
}

void PLUTO_DestroyCoreRegister(PLUTO_CoreRegister_t *reg)
{
    assert(NULL != reg);
    assert(NULL != *reg);
    PLUTO_DestroySharedMemory(&(*reg)->shm);
    PLUTO_Free(*reg);
    *reg = NULL;
}

pid_t PLUTO_CoreRegisterPid(const PLUTO_CoreRegister_t reg, int32_t index)
{
    return (pid_t)atomic_load(&PLUTO_CoreRegisterGetInfo(reg)->nodes[index].pid);
}

unsigned int PLUTO_CoreRegisterFlags(const PLUTO_CoreRegister_t reg, int32_t index)
{
    return atomic_load(&PLUTO_CoreRegisterGetInfo(reg)->nodes[index].flags);
}

const char* PLUTO_CoreRegisterNodeName(const PLUTO_CoreRegister_t reg, int32_t index)
{
    return PLUTO_CoreRegisterGetInfo(reg)->nodes[index].name;
}

const char* PLUTO_CoreRegisterConfigFilename(const PLUTO_CoreRegister_t reg, int32_t index)
{
    return PLUTO_CoreRegisterGetInfo(reg)->nodes[index].config_file;
}

#if !defined(PLUTO_CORE_CLIENT_LIB)

void PLUTO_CoreRegisterSetName(PLUTO_CoreRegister_t reg, int32_t index, const char *name)
{
    const int name_len = strlen(name);
    const int len = name_len < (PLUTO_CORE_REGISTER_NODE_NAME_LEN_MAX - 1) ? name_len : (PLUTO_CORE_REGISTER_NODE_NAME_LEN_MAX - 4);
    memset(PLUTO_CoreRegisterGetInfo(reg)->nodes[index].name, '\0', PLUTO_CORE_REGISTER_NODE_NAME_LEN_MAX);
    memcpy(PLUTO_CoreRegisterGetInfo(reg)->nodes[index].name, name, len); 
    if(name_len >= (PLUTO_CORE_REGISTER_NODE_NAME_LEN_MAX - 1))
    {
        for(int32_t i=0;i<3;++i)
        {
            PLUTO_CoreRegisterGetInfo(reg)->nodes[index].name[len + i] = '.';
        }
    }
}

void PLUTO_CoreRegisterSetConfigFilename(PLUTO_CoreRegister_t reg, int32_t index, const char *filename)
{
    const int name_len = strlen(filename);
    const int len = name_len < (PLUTO_CORE_REGISTER_NODE_CONFIG_FILE_NAME_LEN_MAX - 1) ? name_len : (PLUTO_CORE_REGISTER_NODE_CONFIG_FILE_NAME_LEN_MAX - 4);
    memset(PLUTO_CoreRegisterGetInfo(reg)->nodes[index].config_file, '\0', PLUTO_CORE_REGISTER_NODE_CONFIG_FILE_NAME_LEN_MAX);
    memcpy(PLUTO_CoreRegisterGetInfo(reg)->nodes[index].config_file, filename, len); 
    if(name_len >= (PLUTO_CORE_REGISTER_NODE_CONFIG_FILE_NAME_LEN_MAX - 1))
    {
        for(int32_t i=0;i<3;++i)
        {
            PLUTO_CoreRegisterGetInfo(reg)->nodes[index].config_file[len + i] = '.';
        }
    }
}

void PLUTO_CoreRegisterSetNodePid(PLUTO_CoreRegister_t reg, int32_t index, pid_t pid)
{
    PLUTO_CoreRegisterGetInfo(reg)->nodes[index].pid = pid;
}

void PLUTO_CoreRegisterSetFlags(PLUTO_CoreRegister_t reg, int32_t index, unsigned int flags)
{
    atomic_fetch_or(&PLUTO_CoreRegisterGetInfo(reg)->nodes[index].flags, flags);
}

void PLUTO_CoreRegisterResetFlags(PLUTO_CoreRegister_t reg, int32_t index, unsigned int flags)
{
    atomic_fetch_and(&PLUTO_CoreRegisterGetInfo(reg)->nodes[index].flags, (~flags));
}

#endif 


