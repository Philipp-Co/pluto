#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)

#
# ---------------------------------------------------------------------------------------------------------------------
#
add_subdirectory(
    test
)
#
# ---------------------------------------------------------------------------------------------------------------------
#

#
# Find required Packages.
#
# https://cmake.org/cmake/help/latest/module/FindPython3.html
find_package (Python3 COMPONENTS Interpreter Development)

set(
    PLUTO_INC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_event/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_config/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../pluto_os_abstraction/inc/
    ${CMAKE_CURRENT_LIST_DIR}/../external/
)

set(
    PLUTO_LINK_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_os_abstraction/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_config/
    ${CMAKE_CURRENT_LIST_DIR}/../build/pluto_event/
)

set(
    PLUTO_LINK_LIBS
    pluto_event
    pluto_config
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

#add_subdirectory(
#    test/application_layer/
#)

#
# ---------------------------------------------------------------------------------------------------------------------
#
set(
    SRC
    ${CMAKE_CURRENT_LIST_DIR}/src/application_interface_layer/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/application_layer/pluto_info.c
    ${CMAKE_CURRENT_LIST_DIR}/src/application_layer/pluto_processor.c
)

#
# ---------------------------------------------------------------------------------------------------------------------
#

add_executable(
    pluto_node_pt 
    ${SRC}
)

target_compile_definitions(
    pluto_node_pt 
    PUBLIC
    PLUTO_CTS_RTM_PASSTHROUGH
)

target_include_directories(
    pluto_node_pt
    PUBLIC
    ${PLUTO_INC}
)

target_link_directories(
    pluto_node_pt
    PUBLIC
    ${PLUTO_LINK_DIRS}
)

target_link_libraries(
    pluto_node_pt
    PUBLIC
    ${PLUTO_LINK_LIBS}
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

add_dependencies(
    pluto_node_pt
    pluto_event
    pluto_config
    pluto_os_abstraction
)

#
# ---------------------------------------------------------------------------------------------------------------------
#
add_executable(
    pluto_node_py 
    ${SRC}
    ${CMAKE_CURRENT_LIST_DIR}/src/application_layer/python/pluto_python.c
)

target_compile_definitions(
    pluto_node_py 
    PUBLIC
    PLUTO_CTS_RTM_PYTHON
)

target_include_directories(
    pluto_node_py
    PUBLIC
    ${PLUTO_INC}
    ${Python3_INCLUDE_DIRS}
    #/usr/include/python3.10/
)

target_link_directories(
    pluto_node_py
    PUBLIC
    ${PLUTO_LINK_DIRS}
    #/lib/python3.10/
)

target_link_libraries(
    pluto_node_py
    PUBLIC
    ${PLUTO_LINK_LIBS}
    Python3::Python
    #python3.10.so
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
    pluto_node_sh 
    ${SRC} 
    ${CMAKE_CURRENT_LIST_DIR}/src/application_layer/shared_library/pluto_shared_library.c
)

target_compile_definitions(
    pluto_node_sh 
    PUBLIC
    PLUTO_CTS_RTM_SHARED_LIB
)

target_include_directories(
    pluto_node_sh
    PUBLIC
    ${PLUTO_INC}
    ${Python3_INCLUDE_DIRS}
)

target_link_directories(
    pluto_node_sh
    PUBLIC
    ${PLUTO_LINK_DIRS}
)

target_link_libraries(
    pluto_node_sh 
    PUBLIC 
    ${CMAKE_DL_LIBS}
    ${PLUTO_LINK_LIBS}
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
