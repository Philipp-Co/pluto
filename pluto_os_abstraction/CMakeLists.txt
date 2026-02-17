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
    ${CMAKE_CURRENT_LIST_DIR}/src/message_queue/system_v/pluto_message_queue.c
    #${CMAKE_CURRENT_LIST_DIR}/src/message_queue/custom_0/pluto_custom_0_message_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/src/message_queue/pluto_event.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_semaphore.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_logger.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_shared_memory.c
    ${CMAKE_CURRENT_LIST_DIR}/src/signals/pluto_signal.c
    ${CMAKE_CURRENT_LIST_DIR}/src/system_events/pluto_system_events.c
    ${CMAKE_CURRENT_LIST_DIR}/src/files/pluto_file.c
    ${CMAKE_CURRENT_LIST_DIR}/src/jsmn.c
)

if (${PLUTO_TARGET_OS} EQUAL 2)
    target_compile_definitions(
        pluto_os_abstraction
        PUBLIC
        PLUTO_MESSAGE_QUEUE_SYSTEM_V=1
        #PLUTO_MESSAGE_QUEUE_CUSTOM_0=1
        PLUTO_TARGET_OS=2
        PLUTO_OS_INTERFACE=1
        PLUTO_TEST=1
    )
else()
    target_compile_definitions(
        pluto_os_abstraction
        PUBLIC
        PLUTO_MESSAGE_QUEUE_SYSTEM_V=1
        #PLUTO_MESSAGE_QUEUE_CUSTOM_0=1
        PLUTO_TARGET_OS=1
        PLUTO_OS_INTERFACE=1
        PLUTO_TEST=1
    )
endif()

target_include_directories(
    pluto_os_abstraction
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

target_compile_definitions(
    pluto_os_abstraction
    PUBLIC
    _DARWIN_C_SOURCE=1
    _XOPEN_SOURCE=700
    PLUTO_LOG_LEVEL=2000
)

target_link_options(
    pluto_os_abstraction
    PUBLIC
    "-Wl,-map,output.map"
)

set_property(
    TARGET pluto_os_abstraction
    PROPERTY
    C_STANDARD 11
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
        "-std=c11"
        "-O2"
        "-Wno-unused-function"
        "-fomit-frame-pointer"
        "-fPIC"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-pedantic"
        "-pedantic-errors"
        "-fno-exceptions"
    )
endif()

#
# ---------------------------------------------------------------------------------------------------------------------
#
