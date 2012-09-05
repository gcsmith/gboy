# -----------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    gboy_sdl/CMakeLists.txt
# Created: 06/01/2011
# -----------------------------------------------------------------------------

project(gboy_sdl)

find_package(GLEW   REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2   REQUIRED)

set(SNDLIB ${CMAKE_SOURCE_DIR}/snd_blargg)
set(SNDLIB_INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/snd_blargg
    ${CMAKE_SOURCE_DIR}/snd_blargg/gb_apu
)
set(blargg_src
    ${SNDLIB}/gb_apu/Blip_Buffer.cpp
    ${SNDLIB}/gb_apu/Gb_Apu.cpp
    ${SNDLIB}/gb_apu/Gb_Oscs.cpp
    ${SNDLIB}/gb_apu/Multi_Buffer.cpp
    ${SNDLIB}/Basic_Gb_Apu.cpp
    ${SNDLIB}/Sound_Queue.cpp
)

include_directories(
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    ${SNDLIB_INCLUDE_DIR}
)

add_definitions(
    ${GLEW_DEFINITIONS}
)

set(gboy_sdl_hdr
    graphics.h
    sound.h
)

set(gboy_sdl_src
    graphics.c
    main.c
    sound.cpp
)

set(gboy_sdl_lib
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARY}
    gboy
)

add_executable(gboy_sdl ${gboy_sdl_src} ${blargg_src} ${gboy_sdl_hdr})
target_link_libraries(gboy_sdl ${gboy_sdl_lib})

