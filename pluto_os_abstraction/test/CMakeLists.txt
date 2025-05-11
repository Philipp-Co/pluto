#
# ---------------------------------------------------------------------------------------------------------------------
#

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_os_abstraction_test
    ${CMAKE_CURRENT_LIST_DIR}/src/test_os_abstraction_main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_time.c

    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_time.c
    ${CMAKE_CURRENT_LIST_DIR}/../../external/Unity/src/unity.c
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
