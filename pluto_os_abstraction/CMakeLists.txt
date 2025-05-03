#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)
project(pluto_os_abstraction VERSION 0.0.0 LANGUAGES C)

#
# ---------------------------------------------------------------------------------------------------------------------
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

add_library(
    pluto_os_abstraction
    STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_malloc.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_types.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_message_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_semaphore.c
)

target_include_directories(
    pluto_os_abstraction
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_os_abstraction
        PUBLIC
        "-fno-omit-frame-pointer"
        "-Wno-unused-function"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-pedantic"
        "-pedantic-errors"
    )
    #target_link_options(
    #    pluto_os_abstraction
    #    PUBLIC
    #)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_os_abstraction
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
