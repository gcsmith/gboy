# ------------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    gboy_sdl/CMakeLists.txt
# Created: 06/01/2011
# ------------------------------------------------------------------------------

project(gboy_sdl)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)

include_directories(
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${SDL_INCLUDE_DIR}
)

add_definitions(
    ${GLEW_DEFINITIONS}
)

set(gboy_sdl_hdr
    graphics.h
)

set(gboy_sdl_src
    graphics.c
    main.c
)

set(gboy_sdl_lib
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${SDL_LIBRARY}
    gboy
)

add_executable(gboy_sdl ${MKPROP} ${gboy_sdl_src} ${gboy_sdl_hdr})
target_link_libraries(gboy_sdl ${gboy_sdl_lib})

