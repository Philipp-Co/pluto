
#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)

#
# ---------------------------------------------------------------------------------------------------------------------
#

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
    pluto_info
    ${CMAKE_CURRENT_LIST_DIR}/src/pc_main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pc_info.c
)

target_include_directories(
    pluto_info
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
)

target_link_directories(
    pluto_info
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_os_abstraction/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_config/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_event/
)

target_link_libraries(
    pluto_event
    PUBLIC
    pluto_os_abstraction
    pluto_config
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_info
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
    #    pluto_info
    #    PUBLIC
    #)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_info
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
    pluto_info
    pluto_config
    pluto_event
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

#
# Install Binaries to ${CMAKE_INSTALL_PREFIX}/bin
# https://cmake.org/cmake/help/latest/command/install.html
#
install(
    TARGETS pluto_info DESTINATION bin 
)

#
# ---------------------------------------------------------------------------------------------------------------------
#
