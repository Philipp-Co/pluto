#
# ---------------------------------------------------------------------------------------------------------------------

include(CTest)
enable_testing()
#set(Python3_FIND_STRATEGY LOCATION)#
#set(Python3_FIND_UNVERSIONED_NAMES NEVER)
find_package (Python3 COMPONENTS Interpreter Development)
#find_package (Python3 3.10 COMPONENTS Development)

#
# ---------------------------------------------------------------------------------------------------------------------
#
set(
    SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_processor.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_python.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/application_layer/pluto_processor.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/application_layer/pluto_info.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/application_layer/python/pluto_python.c
    
    ${CMAKE_CURRENT_LIST_DIR}/../../external/Unity/src/unity.c
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_node_test ${SRC}
)

target_compile_definitions(
    pluto_node_test 
    PUBLIC
    PLUTO_CTS_RTM_PASSTHROUGH
    PLUTO_TEST
    PLUTO_TEST_CONFIG="${CMAKE_CURRENT_LIST_DIR}/ressources/test_node_config.txt"
)

target_include_directories(
    pluto_node_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../pluto_edge/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../pluto_config/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../pluto_os_abstraction/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../external/
    ${Python3_INCLUDE_DIRS}
    #/usr/include/python3.10/
)

target_link_directories(
    pluto_node_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../external/Unity/build/
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_edge/
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_event/
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_config/
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_os_abstraction/
    #/lib/python3.10/
)

target_link_libraries(
    pluto_node_test
    PUBLIC
    pluto_edge_static
    pluto_event
    pluto_config
    pluto_os_abstraction
    Python3::Python
    #python3.10.so
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_node_test
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
        pluto_node_test
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_node_test
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

add_test(NAME pluto_node COMMAND pluto_node_test)

#
# ---------------------------------------------------------------------------------------------------------------------
#
