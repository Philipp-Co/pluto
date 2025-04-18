
#include <pluto_info/pc_info.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>


PC_INFO_Info_t PC_INFO_CreateInfo(const char *name)
{
    PC_INFO_Info_t info = malloc(sizeof(struct PC_INFO_Info));
    key_t key = ftok(name, 1);
    int shm_id = shmget(key, 4096, 0);    
    if(shm_id < 0)
    {
        printf("Error, unable to get Shared Memory Segment: %s, for Name %s\n", strerror(errno), name);
        PC_INFO_DestroyInfo(&info);
        goto end;
    }
    info->content = shmat(shm_id, NULL, 0);
    if(!info->content)
    {
        printf("Error, unable to attach to shm: %s\n", strerror(errno));
        PC_INFO_DestroyInfo(&info);
    }
    end:
    return info;
}

void PC_INFO_DestroyInfo(PC_INFO_Info_t *info)
{
    shmdt((*info)->content);
    free(*info);
    *info = NULL;
}

const char* PC_INFO_InfoContent(const PC_INFO_Info_t info)
{
    return info->content;
}
