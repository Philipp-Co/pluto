
#include <pluto_info/pc_info.h>

#include <stdio.h>


int main(int argc, char **argv)
{
    if(argc <= 1)
    {
        printf("Error, provide the Name of the Pluto Instance to get Infromation...\n");
        return -1;
    }
    PC_INFO_Info_t info = PC_INFO_CreateInfo(argv[1]);
    if(info)
    {
        printf("%s\n", PC_INFO_InfoContent(info));    
        PC_INFO_DestroyInfo(&info);
        return 0;
    }
    return -1;
}
