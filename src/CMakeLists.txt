# -----------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    gboy/CMakeLists.txt
# Created: 06/01/2011
# -----------------------------------------------------------------------------

project(libgboy_project)

set(gboy_hdr
    cmdline.h
    common.h
    cpu.h
    gbx.h
    interp.h
    logging.h
    memory.h
    memory_util.h
    ports.h
    romfile.h
    video.h
)

set(gboy_src
    cmdline.c
    debug.c
    gbx.c
    interp.c
    logging.c
    memory.c
    mmu_mbc1.c
    mmu_mbc2.c
    mmu_mbc3.c
    mmu_mbc5.c
    mmu_mbc7.c
    mmu_pcam.c
    romfile.c
    video.c
)

if(WIN32)
    list(APPEND gboy_hdr
        gnulib/getopt.h
        gnulib/getopt_int.h
        gnulib/gettext.h
    )
    list(APPEND gboy_src
        gnulib/getopt.c
        gnulib/getopt1.c
    )
    include_directories(gnulib)
endif(WIN32)

add_library(gboy ${gboy_src} ${gboy_hdr})

# add each sub-directory

if(BUILD_EGL)
    add_subdirectory(egl)
endif(BUILD_EGL)

if(BUILD_SDL)
    add_subdirectory(sdl)
endif(BUILD_SDL)

if(BUILD_WX)
    add_subdirectory(wx)
endif(BUILD_WX)

add_subdirectory(tests)

