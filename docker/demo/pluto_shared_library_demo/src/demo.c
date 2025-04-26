
#include <demo.h>
#include <stdio.h>
#include <string.h>


void PLUTO_SharedLibrarySetupObject(void)
{
    printf("Setup from shared Library!\n");
}

void PLUTO_SharedLibraryTearDownObject(void)
{
    printf("Teardown from shared Library!\n");
}

PLUTO_ProcessorCallbackOutput_t PLUTO_SharedLibraryProcessEvent(PLUTO_ProcessorCallbackInput_t *args)
{
    printf("Run from shared Library!\n");
    snprintf(args->output_buffer, args->output_buffer_size, "{\"shared_lib\":%s}", args->input_buffer);
    PLUTO_ProcessorCallbackOutput_t output = {
        .id = args->id,
        .event = args->event,
        .output_size=strlen(args->output_buffer),
        .return_value=true 
    };
    return output;
}
