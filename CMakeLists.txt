# -----------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    CMakeLists.cmake
# Created: 06/01/2011
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(gboy)

# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_BUILD_TYPE Release)

option(BUILD_EGL "Build the gboy-egl frontend" OFF)
option(BUILD_SDL "Build the gboy-sdl frontend" OFF)
option(BUILD_WX  "Build the gboy-wx frontend" ON)

option(ENABLE_LOG_INFO "Enable 'info' level log messages" ON)
option(ENABLE_LOG_ERR  "Enable 'error' level log messages" ON)
option(ENABLE_LOG_WARN "Enable 'warning' level log messages" OFF)
option(ENABLE_LOG_DBG  "Enable 'debug' level log messages" OFF)
option(ENABLE_LOG_SPEW "Enable 'spew' level log messages" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose type of build: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
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

find_package(Mercurial)
if (MERCURIAL_FOUND)
    MERCURIAL_HG_INFO(${PROJECT_SOURCE_DIR} gboy)
    set(GBOY_VER_HGID ${gboy_HG_ID})
else(MERCURIAL_FOUND)
    set(GBOY_VER_HGID "?hgid?")
endif(MERCURIAL_FOUND)

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

