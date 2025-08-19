#ifndef __PLUTO_TEST_CORE_PREPARE_H__
#define __PLUTO_TEST_CORE_PREPARE_H__

#include <stdbool.h>

const char* PLUTO_TEST_config_path;
const char* PLUTO_binary_dir;

bool PLUTO_TEST_GenerateEmptyCoreConfig(const char *absolute_path_to_file);
void PLUTO_TEST_RemoveGeneratedCoreConfig(const char *absolute_path_to_file);

#endif
