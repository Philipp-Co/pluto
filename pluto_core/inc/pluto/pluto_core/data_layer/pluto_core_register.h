#ifndef __PLUTO_CORE_REGISTER_H__
#define __PLUTO_CORE_REGISTER_H__

#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/os_abstraction/pluto_shared_memory.h>

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>


#define PLUTO_CORE_REGISTER_MAX_NUMBER_OF_NODES 64
#define PLUTO_CORE_REGISTER_PAGE_SIZE_IN_BYTES (4096)
#define PLUTO_CORE_REGISTER_NODE_NAME_LEN_MAX (256)
#define PLUTO_CORE_REGISTER_NODE_CONFIG_FILE_NAME_LEN_MAX ((PLUTO_CORE_REGISTER_PAGE_SIZE_IN_BYTES) - (PLUTO_CORE_REGISTER_NODE_NAME_LEN_MAX) - (2*__SIZEOF_INT__)) 

#include <stdatomic.h>

//
// Size must be 4096 = 1 Page
//
struct PLUTO_CoreRegisterNodeInfo
{
    char config_file[PLUTO_CORE_REGISTER_NODE_CONFIG_FILE_NAME_LEN_MAX];
    char name[PLUTO_CORE_REGISTER_NODE_NAME_LEN_MAX];
    atomic_int pid;
    atomic_uint flags;
} __attribute__((aligned(4096)));

struct PLUTO_CoreInfo
{
    struct PLUTO_CoreRegisterNodeInfo nodes[PLUTO_CORE_REGISTER_MAX_NUMBER_OF_NODES - 1];
    atomic_int nuber_of_nodes; 
} __attribute__((aligned(4096)));

struct PLUTO_CoreRegister
{
    PLUTO_SharedMemory_t shm;
};
typedef struct PLUTO_CoreRegister* PLUTO_CoreRegister_t;

PLUTO_CoreRegister_t PLUTO_CreateCoreRegister(PLUTO_Logger_t logger);
void PLUTO_DestroyCoreRegister(PLUTO_CoreRegister_t *reg);

pid_t PLUTO_CoreRegisterPid(const PLUTO_CoreRegister_t reg, int32_t index);
unsigned int PLUTO_CoreRegisterFlags(const PLUTO_CoreRegister_t reg, int32_t index);
const char* PLUTO_CoreRegisterNodeName(const PLUTO_CoreRegister_t reg, int32_t index);
const char* PLUTO_CoreRegisterConfigFilename(const PLUTO_CoreRegister_t reg, int32_t index);

#if !defined(PLUTO_CORE_CLIENT_LIB)

void PLUTO_CoreRegisterSetName(PLUTO_CoreRegister_t reg, int32_t index, const char *name);
void PLUTO_CoreRegisterSetConfigFilename(PLUTO_CoreRegister_t reg, int32_t index, const char *filename);
void PLUTO_CoreRegisterSetNodePid(PLUTO_CoreRegister_t reg, int32_t index, pid_t pid);
void PLUTO_CoreRegisterSetFlags(PLUTO_CoreRegister_t reg, int32_t index, unsigned int flags);
void PLUTO_CoreRegisterResetFlags(PLUTO_CoreRegister_t reg, int32_t index, unsigned int flags);

#endif

#endif
