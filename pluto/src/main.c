#include "pluto/pluto_config.h"
#include <pluto/pluto_processor.h>
#include <signal.h>
#include <stdatomic.h>


void PLUTO_SignalHandler(int signum);

static atomic_int PLUTO_Terminate;

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    atomic_store(&PLUTO_Terminate, 0);
    signal(SIGINT, PLUTO_SignalHandler);
    
    const char *name = "pluto-0";
    PLUTO_Config_t config = PLUTO_CreateConfig(name);
    if(!config) return -1;
    PLUTO_Processor_t processor = PLUTO_CreateProcessor(
        config
    );
    PLUTO_DestroyConfig(&config);
    if(!processor)
    {
        return -1;
    }
    while(!atomic_load(&PLUTO_Terminate))
    {
        PLUTO_ProcessorProcess(processor);
    }
    PLUTO_DestroyProcessor(&processor);

    return 0;
}

void PLUTO_SignalHandler(int signum)
{
    (void)signum;
    atomic_store(&PLUTO_Terminate, 1);
}
