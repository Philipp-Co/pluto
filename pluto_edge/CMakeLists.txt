#
# ---------------------------------------------------------------------------------------------------------------------
#

enable_testing()

#
# ---------------------------------------------------------------------------------------------------------------------
#

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
set(
    SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_edge.c
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_library(
    pluto_edge_shared SHARED ${SRC}
)

target_compile_definitions(
    pluto_edge_shared
    PUBLIC
    PLUTO_MESSAGE_QUEUE_SYSTEM_V=1
)

target_include_directories(
    pluto_edge_shared
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_event/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_os_abstraction/inc/
)

target_link_directories(
    pluto_edge_shared
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_os_abstraction/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_event/
)

target_link_libraries(
    pluto_edge_shared
    PUBLIC
    pluto_event
    pluto_os_abstraction
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_edge_shared
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
        pluto_edge_shared
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_edge_shared
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

add_library(
    pluto_edge_static STATIC ${SRC}
)

target_compile_definitions(
    pluto_edge_static
    PUBLIC
    PLUTO_MESSAGE_QUEUE_SYSTEM_V=1
)

target_include_directories(
    pluto_edge_static
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_event/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_os_abstraction/inc/
)

target_link_directories(
    pluto_edge_static
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_os_abstraction/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_event/
)

target_link_libraries(
    pluto_edge_static
    PUBLIC
    pluto_event
    pluto_os_abstraction
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_edge_static
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
        pluto_edge_static
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_edge_static
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
    pluto_edge_static
    pluto_event
    pluto_os_abstraction
)

add_dependencies(
    pluto_edge_shared
    pluto_event
    pluto_os_abstraction
)
#
# ---------------------------------------------------------------------------------------------------------------------
#

install(
    TARGETS pluto_edge_shared DESTINATION lib 
)
#
# ---------------------------------------------------------------------------------------------------------------------
#
