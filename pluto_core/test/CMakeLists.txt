#
# ---------------------------------------------------------------------------------------------------------------------
#

include(CTest)
enable_testing()

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_core_test
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_core.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_node_state.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_core_state.c
    ${CMAKE_CURRENT_LIST_DIR}/src/prepare.c

    ${CMAKE_CURRENT_LIST_DIR}/../src/application_layer/pluto_core_state.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/application_layer/pluto_node_state.c
    
    ${CMAKE_CURRENT_LIST_DIR}/../../external/Unity/src/unity.c
)

target_include_directories(
    pluto_core_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../external/
)

target_link_directories(
    pluto_core_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_os_abstraction/
)

target_link_libraries(
    pluto_core_test
    PUBLIC
    pluto_config
    pluto_os_abstraction
)

target_compile_definitions(
    pluto_core_test
    PUBLIC
    PLUTO_CORE_TEST_BINARY_DIR="${CMAKE_CURRENT_LIST_DIR}/build/pluto_node/"
    PLUTO_CORE_TEST_CONFIG_PATH="${CMAKE_CURRENT_LIST_DIR}/ressources/config_files/config.txt"
    PLUTO_CORE_TEST_EXECUTABLE="${CMAKE_CURRENT_LIST_DIR}/ressources/executable_files/loopback"
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_core_test
        PUBLIC
        "-fno-omit-frame-pointer"
        "-Wno-unused-function"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wincompatible-pointer-types"
        "-pedantic"
        "-gdwarf-5"
        "-pedantic-errors"
    )
    #target_link_options(
    #    pluto_core_test
    #    PUBLIC
    #)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_core_test
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
    pluto_core_test
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_test(NAME pluto_core COMMAND pluto_core_test)

#
# ---------------------------------------------------------------------------------------------------------------------
#
