#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)

#
# ---------------------------------------------------------------------------------------------------------------------
#

#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Find required Packages.
#

#
# ---------------------------------------------------------------------------------------------------------------------
#

#
# add unity
#
#add_library(
#    unity STATIC
#    external/Unity/src/unity.c
#)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_core
    ${CMAKE_CURRENT_LIST_DIR}/src/application_interface_layer/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/application_layer/pluto_core.c
    ${CMAKE_CURRENT_LIST_DIR}/src/application_layer/pluto_node_state.c
    ${CMAKE_CURRENT_LIST_DIR}/src/data_layer/pluto_sig_queue.c
)

target_include_directories(
    pluto_core
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

target_link_directories(
    pluto_core
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_config/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_os_abstraction/
)

target_link_libraries(
    pluto_core
    PUBLIC
    pluto_config
    pluto_os_abstraction
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_core
        PUBLIC
        "-fno-omit-frame-pointer"
        "-Wno-unused-function"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-pedantic"
        "-gdwarf-5"
        "-pedantic-errors"
    )
    #target_link_options(
    #    pluto_core
    #    PUBLIC
    #)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_core
        PUBLIC
        "-O2"
        "-Wno-unused-function"
        "-fPIC"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-pedantic"
        "-pedantic-errors"
    )
endif()

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_dependencies(
    pluto_core
    pluto_config
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#
