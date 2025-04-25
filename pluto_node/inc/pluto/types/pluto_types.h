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
} PLUTO_ProcessorCallbackInput_t;

typedef struct
{
    size_t output_size;
    bool return_value;
} PLUTO_ProcessorCallbackOutput_t;

#endif
