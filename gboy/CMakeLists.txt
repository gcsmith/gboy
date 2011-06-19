# ------------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    gboy/CMakeLists.txt
# Created: 06/01/2011
# ------------------------------------------------------------------------------

project(libgboy_project)

set(gboy_hdr
    cmdline.h
    common.h
    gbx.h
    ports.h
    romfile.h
)

set(gboy_src
    cmdline.c
    debug.c
    gbx.c
    interp.c
    memory.c
    mmu_mbc1.c
    mmu_mbc2.c
    mmu_mbc3.c
    mmu_mbc5.c
    romfile.c
    tables.c
    video.c
)

if(WIN32)
    # use gnulib for windows portability
    list(APPEND gboy_hdr
        ${GNULIB_DIR}/getopt.h
        ${GNULIB_DIR}/getopt_int.h
        ${GNULIB_DIR}/gettext.h
    )
    list(APPEND gboy_src
        ${GNULIB_DIR}/getopt.c
        ${GNULIB_DIR}/getopt1.c
    )
    include_directories(${GNULIB_DIR})
endif()

add_library(gboy ${gboy_src} ${gboy_hdr})

