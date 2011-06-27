# -----------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    gboy_wx/CMakeLists.txt
# Created: 06/25/2011
# -----------------------------------------------------------------------------

project(gboy_wx)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS gl core base)

include(${wxWidgets_USE_FILE})

include_directories(
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

add_definitions(
    ${GLEW_DEFINITIONS}
)

set(gboy_wx_hdr
    gbxThread.h
    GboyApp.h
    MainFrame.h
    RenderWidget.h
)

set(gboy_wx_src
    gbxThread.cpp
    GboyApp.cpp
    MainFrame.cpp
    RenderWidget.cpp
)

set(gboy_wx_lib
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${wxWidgets_LIBRARIES}
    gboy
)

add_executable(gboy_wx ${gboy_wx_src} ${gboy_wx_hdr})
target_link_libraries(gboy_wx ${gboy_wx_lib})

