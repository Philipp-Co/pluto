
#
# ---------------------------------------------------------------------------------------------------------------------
#

include(CTest)
enable_testing()

#
# ---------------------------------------------------------------------------------------------------------------------
#
set(
    SRC
    ${CMAKE_CURRENT_LIST_DIR}/../src/pluto_edge.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_edge_main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/test_pluto_edge.c
    ${CMAKE_CURRENT_LIST_DIR}/../../external/Unity/src/unity.c
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_edge_test ${SRC}
)

#target_compile_definitions(
#    pluto_edge_test
#    PUBLIC
#    PLUTO_TEST=1
#)

target_include_directories(
    pluto_edge_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../inc/
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../../external/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_event/inc/
)

target_link_directories(
    pluto_edge_test
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_event/
    ${CMAKE_CURRENT_LIST_DIR}/../../build/pluto_os_abstraction/
)

target_link_libraries(
    pluto_edge_test
    PUBLIC
    pluto_event
    pluto_os_abstraction
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_edge_test
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
        pluto_edge_test
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_edge_test
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
add_dependencies(
    pluto_edge_test
    pluto_event
    pluto_os_abstraction
)
#
# ---------------------------------------------------------------------------------------------------------------------
#

add_test(NAME pluto_edge COMMAND pluto_edge_test)

#
# ---------------------------------------------------------------------------------------------------------------------
#
