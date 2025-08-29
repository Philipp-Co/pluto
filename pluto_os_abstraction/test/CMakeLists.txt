#
# ---------------------------------------------------------------------------------------------------------------------
#

include(CTest)
enable_testing()

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_os_abstraction_test
    ${CMAKE_CURRENT_LIST_DIR}/src/test_os_abstraction_main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_time.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_os_abstraction_semaphore.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_os_abstraction_messagequeue.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_system_events.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_signals.c

    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_time.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/message_queue/system_v/pluto_message_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_semaphore.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/system_events/pluto_system_events.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/signals/pluto_signal.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/files/pluto_file.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_logger.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_types.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_malloc.c
    ${CMAKE_CURRENT_LIST_DIR}/../../external/Unity/src/unity.c
)

target_compile_definitions(
    pluto_os_abstraction_test 
    PUBLIC
    #PLUTO_TEST=1
    PLUTO_TEST_RESSOURCE_DIR="${CMAKE_CURRENT_LIST_DIR}/ressources/"
)


target_include_directories(
    pluto_os_abstraction_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../inc/
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../external/
)

target_compile_definitions(
    pluto_os_abstraction_test
    PUBLIC
    _XOPEN_SOURCE=700
    PLUTO_MESSAGE_QUEUE_SYSTEM_V=1
)

target_compile_options(
    pluto_os_abstraction_test
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

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_test(NAME os_abstraction COMMAND pluto_os_abstraction_test)

#
# ---------------------------------------------------------------------------------------------------------------------
#
