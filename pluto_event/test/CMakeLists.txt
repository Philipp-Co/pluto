#
# ---------------------------------------------------------------------------------------------------------------------
#

include(CTest)
enable_testing()

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
    pluto_event_test
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_event.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_event_main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/jsmn.c

    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_event.c

    ${CMAKE_CURRENT_LIST_DIR}/../../external/Unity/src/unity.c
)

target_include_directories(
    pluto_event_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../inc/
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../external/
)

target_link_directories(
    pluto_event_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_os_abstraction/
)

target_link_libraries(
    pluto_event_test
    PUBLIC
    pluto_os_abstraction
)

#
# Configure libahr.
#
target_compile_options(
    pluto_event_test
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

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_dependencies(
    pluto_event_test
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_test(NAME pluto_event COMMAND pluto_event_test)

#
# ---------------------------------------------------------------------------------------------------------------------
#
