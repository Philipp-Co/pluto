#ifndef __PLUTO_CLIENT_INFO_H__
#define __PLUTO_CLIENT_INFO_H__

struct PC_INFO_Info
{
    char *content;
};
typedef struct PC_INFO_Info* PC_INFO_Info_t;

PC_INFO_Info_t PC_INFO_CreateInfo(const char *name);
void PC_INFO_DestroyInfo(PC_INFO_Info_t *info);
const char* PC_INFO_InfoContent(const PC_INFO_Info_t info);

#endif
