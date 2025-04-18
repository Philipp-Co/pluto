#
# ---------------------------------------------------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.16)

#
# ---------------------------------------------------------------------------------------------------------------------
#

enable_testing()

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
find_package (Python3 REQUIRED COMPONENTS Interpreter Development)
message(
    STATUS
    "Python Found: ${Python3_FOUND} - Python Version: ${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}.${Python3_VERSION_PATCH}"
)
message(
    STATUS
    "System Interpreter found: ${Python3_Interpreter_FOUND}"
)
message(
    STATUS
    "Dev found: ${Python3_Development_FOUND}"
)
message(
    STATUS
    "Include Dirs: ${Python3_INCLUDE_DIRS}"
)
message(
    STATUS
    "Executable: ${Python3_EXECUTABLE}"
)
message(
    STATUS
    "Stdlib: ${Python3_STDLIB}"
)

if(${Python3_Interpreter_FOUND})
    message(
        STATUS
        "INTERPRETER FOUND!"
    )
else()
    message(
        STATUS
        "INTERPRETER --NOT-- FOUND!"
    )
endif()

if(${Python3_Development_FOUND})
    message(
        STATUS
        "STDARCH: ${Python3_STDARCH}"
    )
else()
    message(
        STATUS
        "Development --NOT-- FOUND!"
    )
endif()

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
)

target_link_directories(
    pluto_node_py
    PUBLIC
    ${PLUTO_LINK_DIRS}
)

target_link_libraries(
    pluto_node_py
    PUBLIC
    ${PLUTO_LINK_LIBS}
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

#
# Install Binaries to ${CMAKE_INSTALL_PREFIX}/bin
# https://cmake.org/cmake/help/latest/command/install.html
#
install(
    TARGETS pluto_node_sh pluto_node_pt pluto_node_py DESTINATION bin 
)

#
# ---------------------------------------------------------------------------------------------------------------------
#
