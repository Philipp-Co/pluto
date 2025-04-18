#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)
project(pluto_shared_library_demo VERSION 0.0.0 LANGUAGES C)

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
    ${CMAKE_CURRENT_LIST_DIR}/src/demo.c
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_library(
    pluto_demo SHARED ${SRC}
)

target_compile_definitions(
    pluto_demo 
    PUBLIC
    PLUTO_CTS_RTM_SHARED_LIBRARY
)

target_include_directories(
    pluto_demo
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../../pluto_node/inc/
    /usr/include/pluto/inc/
)

#
# Configure libahr.
#
target_compile_options(
    pluto_demo
    PUBLIC
    "-fPIC"
    "-Wno-unused-function"
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wincompatible-pointer-types"
    "-pedantic"
    "-pedantic-errors"
)
#target_link_options(
#    pluto_demo
#    PUBLIC
#)

#
# ---------------------------------------------------------------------------------------------------------------------
#
