
#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)
project(pluto VERSION 0.0.0 LANGUAGES C)

#
# ---------------------------------------------------------------------------------------------------------------------
#

enable_testing()

#
# ---------------------------------------------------------------------------------------------------------------------
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(
    pluto_os_abstraction
)

add_subdirectory(
    pluto_event
)

add_subdirectory(
    pluto_config
)

add_subdirectory(
    pluto_core
)

add_subdirectory(
    pluto_info
)

add_subdirectory(
    pluto_rw
)

add_subdirectory(
    pluto_node
)

add_subdirectory(
    pluto_edge
)

#
# ---------------------------------------------------------------------------------------------------------------------
#



#
# ---------------------------------------------------------------------------------------------------------------------
#
