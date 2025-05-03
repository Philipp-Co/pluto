#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)
project(pluto_test_application_layer VERSION 0.0.0 LANGUAGES C)

#
# ---------------------------------------------------------------------------------------------------------------------
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Find required Packages.
#
# https://cmake.org/cmake/help/latest/module/FindPython3.html

#
# ---------------------------------------------------------------------------------------------------------------------
#

#
# ---------------------------------------------------------------------------------------------------------------------
#
set(
    SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/../../src/jsmn.c
    ${CMAKE_CURRENT_LIST_DIR}/../../src/application_layer/pluto_config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_config.c
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_test_application_layer ${SRC}
)

target_compile_definitions(
    pluto_test_application_layer 
    PUBLIC
    PLUTO_CTS_RTM_PASSTHROUGH
    PLUTO_TEST
)

target_include_directories(
    pluto_test_application_layer
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../../external/
)

target_link_directories(
    pluto_test_application_layer
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../../external/Unity/build/
)

target_link_libraries(
    pluto_test_application_layer
    PUBLIC
    unity
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_test_application_layer
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
        pluto_test_application_layer
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_test_application_layer
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
