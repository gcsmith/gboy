# -----------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    CMakeLists.cmake
# Created: 06/01/2011
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)
project(gboy)

# set default project options

option(BUILD_EGL "Build the gboy-egl frontend" OFF)
option(BUILD_SDL "Build the gboy-sdl frontend" ON)
option(BUILD_WX  "Build the gboy-wx frontend"  ON)

option(ENABLE_LOG_INFO    "Enable log message level: info"    ON)
option(ENABLE_LOG_ERROR   "Enable log message level: error"   ON)
option(ENABLE_LOG_WARNING "Enable log message level: warning" OFF)
option(ENABLE_LOG_DEBUG   "Enable log message level: debug"   OFF)
option(ENABLE_LOG_VERBOSE "Enable log message level: verbose" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Build type: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(GBOY_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_MODULE_PATH ${GBOY_MODULE_PATH} ${CMAKE_MODULE_PATH})

# detect the platform and architecture

include(${GBOY_MODULE_PATH}/DetectPlatform.cmake)
include(${GBOY_MODULE_PATH}/DetectArchitecture.cmake)

# encode version number as <major>.<minor>.<patch>:<changeset>

set(GBOY_VERSION_MAJOR "0")
set(GBOY_VERSION_MINOR "0")
set(GBOY_VERSION_PATCH "1")

find_package(Git)
if(GIT_FOUND)
    GIT_WC_INFO(${PROJECT_SOURCE_DIR} GBOY)
    set(GBOY_VERSION_EXTRA ${GBOY_WC_REVISION_HASH})
endif(GIT_FOUND)

set(BUILD_VERSION "${GBOY_VERSION_MAJOR}.${GBOY_VERSION_MINOR}.${GBOY_VERSION_PATCH}:${GBOY_VERSION_EXTRA}")

# perform configuration and generate config.h file

include(CheckIncludeFiles)
check_include_files(intrin.h HAVE_INTRIN_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(windows.h HAVE_WINDOWS_H)

include(CheckFunctionExists)
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)

configure_file(
    ${CMAKE_SOURCE_DIR}/src/config.h.in
    ${CMAKE_BINARY_DIR}/src/config.h
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

message(STATUS "--------------------------------------------------------------")
message(STATUS "CMAKE_BUILD_TYPE:       ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_VERBOSE_MAKEFILE: ${CMAKE_VERBOSE_MAKEFILE}")
message(STATUS "BUILD_VERSION:          ${BUILD_VERSION}")
message(STATUS "BUILD_EGL:              ${BUILD_EGL}")          
message(STATUS "BUILD_SDL:              ${BUILD_SDL}")
message(STATUS "BUILD_WX:               ${BUILD_WX}")
message(STATUS "ENABLE_LOG_INFO:        ${ENABLE_LOG_INFO}")
message(STATUS "ENABLE_LOG_ERROR:       ${ENABLE_LOG_ERROR}")
message(STATUS "ENABLE_LOG_WARNING:     ${ENABLE_LOG_WARNING}")
message(STATUS "ENABLE_LOG_DEBUG:       ${ENABLE_LOG_DEBUG}")
message(STATUS "ENABLE_LOG_VERBOSE:     ${ENABLE_LOG_VERBOSE}")
message(STATUS "--------------------------------------------------------------")

# add each sub-directory

add_subdirectory(src)

