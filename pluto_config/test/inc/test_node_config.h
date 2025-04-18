#ifndef __TEST_PLUTO_NODE_CONFIG_H__
#define __TEST_PLUTO_NODE_CONFIG_H__

///
/// \brief  Check if a Config can be created with a Config File.
///         Assume the Config File is empty. 
///
void test_initial(void);
///
/// \brief  Check if a Config can be created with a minimal Config File.
///             - work_dir is set
///             - Inputqueue Name is set to a Name with length > 0
///             - One Output Queue is given in the Output Queues List with a Name and length > 0
///
void test_minimal_config(void);
///
/// \brief  Check if a Config can be created with multiple Output Queues.
///             - work_dir is set
///             - Inputqueue Name is set to a Name with length > 0
///             - Three Output Queues are given in the Output Queues List with a Name and length > 0
///
void test_multiple_output_queues_config(void);
///
/// \brief  Check if a Config can be created, when the "python_path" is given.
///
void test_python_config(void);

#endif
