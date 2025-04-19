
#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.25)
project(pluto VERSION 0.0.0 LANGUAGES C)

#
# ---------------------------------------------------------------------------------------------------------------------
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(
    pluto_info
)
add_subdirectory(
    pluto_rw
)

#
# Find required Packages.
#
# https://cmake.org/cmake/help/latest/module/FindPython3.html
find_package (Python3 COMPONENTS Interpreter Development)

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
    pluto
    ${CMAKE_CURRENT_LIST_DIR}/pluto/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/pluto/src/pluto_types.c
    ${CMAKE_CURRENT_LIST_DIR}/pluto/src/pluto_info.c
    ${CMAKE_CURRENT_LIST_DIR}/pluto/src/pluto_config.c
    ${CMAKE_CURRENT_LIST_DIR}/pluto/src/pluto_message_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/pluto/src/pluto_message_parser.c
    ${CMAKE_CURRENT_LIST_DIR}/pluto/src/pluto_processor.c
    ${CMAKE_CURRENT_LIST_DIR}/pluto/src/python/pluto_python.c
)

target_include_directories(
    pluto
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/pluto/inc/
    ${CMAKE_CURRENT_LIST_DIR}/external/
    ${Python3_INCLUDE_DIRS}
)

target_link_libraries(
    pluto
    PUBLIC
    Python3::Python
)
#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto
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
        pluto
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto
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
