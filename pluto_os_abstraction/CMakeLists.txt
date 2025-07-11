#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)

#
# ---------------------------------------------------------------------------------------------------------------------
#

include(CTest)
enable_testing()
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
# ---------------------------------------------------------------------------------------------------------------------
#

add_library(
    pluto_os_abstraction
    STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_malloc.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_time.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_types.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_message_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_semaphore.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_logger.c
)

target_include_directories(
    pluto_os_abstraction
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

target_compile_definitions(
    pluto_os_abstraction
    PUBLIC
    _XOPEN_SOURCE=700
    PLUTO_LOG_LEVEL=2000
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_os_abstraction
        PUBLIC
        "-std=c11"
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

