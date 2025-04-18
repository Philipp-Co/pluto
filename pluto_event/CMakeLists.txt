#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)
project(pluto_event VERSION 0.0.0 LANGUAGES C)

#
# ---------------------------------------------------------------------------------------------------------------------
#

enable_testing()

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

add_subdirectory(
    test/
)

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
    pluto_event
    STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_event.c
    ${CMAKE_CURRENT_LIST_DIR}/src/jsmn.c
)

target_include_directories(
    pluto_event
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

target_link_directories(
    pluto_event
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_os_abstraction/
)

target_link_libraries(
    pluto_event
    PUBLIC
    pluto_os_abstraction
)

target_compile_definitions(
    pluto_event
    PUBLIC
    _XOPEN_SOURCE=700
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_event
        PUBLIC
        "-std=c11"
        "-fno-omit-frame-pointer"
        "-Wno-unused-function"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-pedantic"
        "-pedantic-errors"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_event
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
    pluto_event
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

install(
    TARGETS pluto_event DESTINATION lib 
)
#
# ---------------------------------------------------------------------------------------------------------------------
#
