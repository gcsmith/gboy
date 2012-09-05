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
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

set(GBOY_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_MODULE_PATH ${GBOY_MODULE_PATH} ${CMAKE_MODULE_PATH})
set(GNULIB_DIR "${CMAKE_SOURCE_DIR}/gnulib")

# detect the platform and architecture

include(${GBOY_MODULE_PATH}/DetectPlatform.cmake)
include(${GBOY_MODULE_PATH}/DetectArchitecture.cmake)

# encode version number as <major>.<minor>.<revision>:<changeset>

set(GBOY_VER_MAJ "0")
set(GBOY_VER_MIN "0")
set(GBOY_VER_REV "1")

find_package(Git)
if(GIT_FOUND)
    GIT_WC_INFO(${PROJECT_SOURCE_DIR} GBOY)
    set(GBOY_VER_ID ${GBOY_WC_REVISION_HASH})
endif(GIT_FOUND)

set(BUILD_VERSION "${GBOY_VER_MAJ}.${GBOY_VER_MIN}.${GBOY_VER_REV}:${GBOY_VER_ID}")

# perform configuration and generate config.h file

include(CheckIncludeFiles)
check_include_files(intrin.h HAVE_INTRIN_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(windows.h HAVE_WINDOWS_H)

include(CheckFunctionExists)
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)

configure_file(
    ${CMAKE_SOURCE_DIR}/gboy/config.h.cmake
    ${CMAKE_BINARY_DIR}/gboy/config.h
)

include_directories(
    "${CMAKE_SOURCE_DIR}/gboy"
    "${CMAKE_BINARY_DIR}/gboy"
)

message(STATUS "--------------------------------------------------------------")
message(STATUS "CMAKE_BUILD_TYPE:       ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_VERBOSE_MAKEFILE: ${CMAKE_VERBOSE_MAKEFILE}")
message(STATUS "BUILD_EGL:              ${BUILD_EGL}")          
message(STATUS "BUILD_SDL:              ${BUILD_SDL}")
message(STATUS "BUILD_WX:               ${BUILD_WX}")
message(STATUS "BUILD_VERSION:          ${BUILD_VERSION}")
message(STATUS "--------------------------------------------------------------")

set(BUILD_EGL ${BUILD_EGL} CACHE BOOL
    "Build the gboy-egl frontend" FORCE)
set(BUILD_SDL ${BUILD_SDL} CACHE BOOL
    "Build the gboy-sdl frontend" FORCE)
set(BUILD_WX ${BUILD_WX} CACHE BOOL
    "Build the gboy-wx frontend" FORCE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING
    "Choose type of build: None Debug Release RelWithDebInfo MinSizeRel" FORCE)

# add each sub-project
add_subdirectory(gboy)

if(BUILD_EGL)
    add_subdirectory(gboy_egl)
endif(BUILD_EGL)

if(BUILD_SDL)
    add_subdirectory(gboy_sdl)
endif(BUILD_SDL)

if(BUILD_WX)
    add_subdirectory(gboy_wx)
endif(BUILD_WX)

