#
# ---------------------------------------------------------------------------------------------------------------------
#

include(CTest)
enable_testing()

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_config_test
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_node_config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_core_config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/utils.c
    
    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_config.c

    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_core_config.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/private/src/pluto_file.c
    
    ${CMAKE_CURRENT_LIST_DIR}/../../external/Unity/src/unity.c
)

target_include_directories(
    pluto_config_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../inc/
    ${CMAKE_CURRENT_LIST_DIR}/../src/private/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../external/
)

target_link_directories(
    pluto_config_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_os_abstraction/
)

target_link_libraries(
    pluto_config_test
    PUBLIC
    pluto_os_abstraction
)

target_compile_definitions(
    pluto_config_test
    PUBLIC
    # Path to the Ressources Directory. Tests will use this Directory for Files etc.
    TEST_PLUTO_CONFIG_RESSOURCE_DIR="${CMAKE_CURRENT_LIST_DIR}/ressources/"
    # Path to the Workingdirektory.
    TEST_PLUTO_WORKDIR="${CMAKE_BINARY_DIR}/test_workdir/"
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_config_test
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
    #    pluto_config_test
    #    PUBLIC
    #)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_config_test
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
    pluto_config_test
    pluto_os_abstraction
)
#
# ---------------------------------------------------------------------------------------------------------------------
#

#
add_test(NAME pluto_config COMMAND pluto_config_test)

#
# ---------------------------------------------------------------------------------------------------------------------
#
