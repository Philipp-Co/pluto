#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)
project(pluto VERSION 0.0.0 LANGUAGES C)

#
# ---------------------------------------------------------------------------------------------------------------------
#

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Find required Packages.
#
# https://cmake.org/cmake/help/latest/module/FindPython3.html
find_package (Python3 COMPONENTS Interpreter Development)

#
# ---------------------------------------------------------------------------------------------------------------------
#

#
# ---------------------------------------------------------------------------------------------------------------------
#
set(
    SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_malloc.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_types.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_info.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_config.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_message_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_message_parser.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_processor.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pluto_semaphore.c
    ${CMAKE_CURRENT_LIST_DIR}/src/jsmn.c
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_node_pt ${SRC}
)

target_compile_definitions(
    pluto_node_pt 
    PUBLIC
    PLUTO_CTS_RTM_PASSTHROUGH
)

target_include_directories(
    pluto_node_pt
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_node_pt
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
        pluto_node_pt
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_node_pt
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
add_executable(
    pluto_node_py ${SRC} ${CMAKE_CURRENT_LIST_DIR}/src/python/pluto_python.c
)

target_compile_definitions(
    pluto_node_py 
    PUBLIC
    PLUTO_CTS_RTM_PYTHON
)

target_include_directories(
    pluto_node_py
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
    ${Python3_INCLUDE_DIRS}
)

target_link_libraries(
    pluto_node_py
    PUBLIC
    Python3::Python
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_node_py
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
        pluto_node_py
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_node_py
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

add_executable(
    pluto_node_sh ${SRC} ${CMAKE_CURRENT_LIST_DIR}/src/shared_library/pluto_shared_library.c
)

target_compile_definitions(
    pluto_node_sh 
    PUBLIC
    PLUTO_CTS_RTM_SHARED_LIB
)

target_include_directories(
    pluto_node_sh
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
    ${Python3_INCLUDE_DIRS}
)

target_link_libraries(
    pluto_node_sh PUBLIC ${CMAKE_DL_LIBS}
)

#
# Configure libahr.
#
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        pluto_node_sh
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
        pluto_node_sh
        PUBLIC
        "-fsanitize=address"
        "-fno-omit-frame-pointer"
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(
        pluto_node_sh
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
