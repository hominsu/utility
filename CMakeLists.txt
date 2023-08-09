CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
if (POLICY CMP0025)
    # detect Apple's Clang
    cmake_policy(SET CMP0025 NEW)
endif ()
if (POLICY CMP0040)
    cmake_policy(SET CMP0040 NEW)
endif ()
if (POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif ()

set(LIB_MAJOR_VERSION "1")
set(LIB_MINOR_VERSION "0")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

if (CMAKE_VERSION VERSION_LESS 3.0)
    PROJECT(utility CXX)
else ()
    cmake_policy(SET CMP0048 NEW)
    PROJECT(utility VERSION "${LIB_VERSION_STRING}" LANGUAGES C CXX)
endif ()

# compile in release with debug info mode by default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# Build all binaries in a separate directory
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(UTILITY_ENABLE_INSTRUMENTATION_OPT "Build utility with -march or -mcpu options" ON)
option(UTILITY_BUILD_ASAN "Build utility with address sanitizer (gcc/clang)" OFF)
option(UTILITY_BUILD_UBSAN "Build utility with undefined behavior sanitizer (gcc/clang)" OFF)
option(UTILITY_BUILD_EXAMPLES "Build utility examples." ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

message("")
message("Operation system is ${CMAKE_SYSTEM}")
message("Current compiler: ${CMAKE_CXX_COMPILER_ID}")
message("Current compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message("Current compiler directory: ${CMAKE_CXX_COMPILER}")

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
    endif ()
endif (CCACHE_FOUND)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (UTILITY_ENABLE_INSTRUMENTATION_OPT AND NOT CMAKE_CROSSCOMPILING)
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "powerpc" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc64le")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=native")
        else ()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        endif ()
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    set(EXTRA_CXX_FLAGS -Weffc++ -Wswitch-default -Wfloat-equal -Wconversion -Wsign-conversion)
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    endif ()
    if (UTILITY_BUILD_ASAN)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.0")
            message(FATAL_ERROR "GCC < 4.8 doesn't support the address sanitizer")
        else ()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
        endif ()
    endif ()
    if (UTILITY_BUILD_UBSAN)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0")
            message(FATAL_ERROR "GCC < 4.9 doesn't support the undefined behavior sanitizer")
        else ()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
        endif ()
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (UTILITY_ENABLE_INSTRUMENTATION_OPT AND NOT CMAKE_CROSSCOMPILING)
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "powerpc" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "ppc64le")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=native")
        else ()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        endif ()
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    set(EXTRA_CXX_FLAGS -Weffc++ -Wswitch-default -Wfloat-equal -Wconversion -Wimplicit-fallthrough)
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.0")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    endif ()
    if (UTILITY_BUILD_ASAN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif ()
    if (UTILITY_BUILD_UBSAN)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined-trap -fsanitize-undefined-trap-on-error")
        else ()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
        endif ()
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
    add_definitions(-DNOMINMAX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.14")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    endif ()
    # Always compile with /WX
    if (CMAKE_CXX_FLAGS MATCHES "/WX-")
        string(REGEX REPLACE "/WX-" "/WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "XL")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qarch=auto")
endif ()

add_subdirectory(src)
add_subdirectory(example)

#add extra search paths for libraries and includes
SET(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "The directory the headers are installed in")
SET(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "Directory where lib will install")

# header
install(DIRECTORY include/utility
        DESTINATION "${INCLUDE_INSTALL_DIR}"
        COMPONENT dev)

# cmake
if (UNIX OR CYGWIN)
    set(_CMAKE_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}")
elseif (WIN32)
    set(_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/cmake")
endif ()
set(CMAKE_INSTALL_DIR "${_CMAKE_INSTALL_DIR}" CACHE PATH "The directory cmake files are installed in")

export(PACKAGE ${PROJECT_NAME})

set(CONFIG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(${PROJECT_NAME}_INCLUDE_DIR "\${${PROJECT_NAME}_SOURCE_DIR}/include")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        @ONLY)
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}ConfigVersion.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        @ONLY)

set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME})
file(RELATIVE_PATH REL_INCLUDE_DIR
        "${CMAKECONFIG_INSTALL_DIR}"
        "${CMAKE_INSTALL_PREFIX}/include")
set(${PROJECT_NAME}_INCLUDE_DIR "\${${PROJECT_NAME}_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONFIG_SOURCE_DIR)
set(CONFIG_DIR)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

if (CMAKE_INSTALL_DIR)
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            DESTINATION "${CMAKE_INSTALL_DIR}"
            COMPONENT dev)
endif ()

# pkg-config
IF (UNIX OR CYGWIN)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
            @ONLY)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
            DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
            COMPONENT pkgconfig)
ENDIF ()

# uninstall target
if (NOT TARGET uninstall)
    configure_file(
            "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Uninstall.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake"
            IMMEDIATE @ONLY)
    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Uninstall.cmake)
endif ()

message("The directory the headers are installed in: ${CMAKE_INSTALL_PREFIX}")
message("Directory where lib will install: ${LIB_INSTALL_DIR}")
message("")
