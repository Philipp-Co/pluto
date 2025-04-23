#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)
project(pluto VERSION 0.0.0 LANGUAGES C)

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
# ---------------------------------------------------------------------------------------------------------------------
#
set(
    SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_edge.c
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_library(
    pluto_edge_shared SHARED ${SRC}
)

target_include_directories(
    pluto_edge_shared
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_edge_shared
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
        "-Wno-unused-function"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-pedantic"
        "-pedantic-errors"
    )
    target_link_options(
        pluto_edge_shared
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_edge_shared
        PUBLIC
        "-O2"
        "-fdata-sections"
        "-ffunction-sections"
        "-Wno-unused-function"
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

add_library(
    pluto_edge_static STATIC ${SRC}
)

target_include_directories(
    pluto_edge_static
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_edge_static
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
        "-Wno-unused-function"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-pedantic"
        "-pedantic-errors"
    )
    target_link_options(
        pluto_edge_static
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_edge_static
        PUBLIC
        "-O2"
        "-fdata-sections"
        "-ffunction-sections"
        "-Wno-unused-function"
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
