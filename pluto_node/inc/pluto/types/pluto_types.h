#ifndef __PUBLIC_PLUTO_TYPES_H__
#define __PUBLIC_PLUTO_TYPES_H__

#include <stddef.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    char *input_buffer;
    char *output_buffer;
    size_t input_buffer_size;
    size_t output_buffer_size;
    uint32_t id;
    uint32_t event;
    uint8_t number_of_output_queues;
} PLUTO_ProcessorCallbackInput_t __attribute__((aligned(64)));

typedef struct
{
    uint64_t output_to_queues;
    size_t output_size;
    uint32_t id;
    uint32_t event;
    bool return_value;
} PLUTO_ProcessorCallbackOutput_t __attribute__((aligned(64)));

#endif
