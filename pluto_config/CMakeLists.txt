#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)

#
# ---------------------------------------------------------------------------------------------------------------------
#

enable_testing()
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Find required Packages.
#

add_subdirectory(
    test/
)

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
    pluto_config
    STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/private/src/pluto_file.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_core_config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/jsmn.c
)

target_include_directories(
    pluto_config
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/src/private/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

target_link_directories(
    pluto_config
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_os_abstraction/
)

target_link_libraries(
    pluto_config
    PUBLIC
    pluto_os_abstraction
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_config
        PUBLIC
        "-fno-omit-frame-pointer"
        "-Wno-unused-function"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-gdwarf-5"
        "-pedantic"
        "-pedantic-errors"
    )
    #target_link_options(
    #    pluto_config
    #    PUBLIC
    #)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_config
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
    pluto_config
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#
