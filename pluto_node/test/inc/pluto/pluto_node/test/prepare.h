#ifndef __PLUTO_TEST_PREPARE_H__
#define __PLUTO_TEST_PREPARE_H__

#include <pluto/os_abstraction/pluto_logger.h>
#include <pluto/pluto_config/pluto_config.h>
#include <pluto/pluto_edge/pluto_edge.h>
#include <stddef.h>

extern PLUTO_Logger_t PLUTO_TEST_edge_logger;
extern PLUTO_Logger_t PLUTO_TEST_processor_logger;

extern const char *PLUTO_TEST_name;
extern PLUTO_Config_t PLUTO_TEST_config;
    
extern const char *PLUTO_TEST_python_path;
extern const char *PLUTO_TEST_executable;

#endif
