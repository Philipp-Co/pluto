
#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_rw
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/prw_message_queue.c
)

target_include_directories(
    pluto_rw
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_edge/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_event/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_os_abstraction/inc/
)

target_link_directories(
    pluto_rw
    PUBLIC
    #${CMAKE_CURRENT_LIST_DIR}/../build/pluto_os_abstraction/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_config/
    #${CMAKE_CURRENT_LIST_DIR}/../build/pluto_event/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_edge/
)

target_link_libraries(
    pluto_rw
    PUBLIC
    pluto_event
    pluto_edge_static
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_rw
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
    #target_link_options(
    #    pluto_rw
    #    PUBLIC
    #)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_rw
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
    pluto_rw
    pluto_config
    pluto_edge_static
    pluto_event
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#
