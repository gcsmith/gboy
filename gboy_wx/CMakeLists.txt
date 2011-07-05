# -----------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    gboy_wx/CMakeLists.txt
# Created: 06/25/2011
# -----------------------------------------------------------------------------

project(gboy_wx)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS gl core base adv xrc xml html)

include(${wxWidgets_USE_FILE})

include_directories(
    ${CMAKE_BINARY_DIR}/gboy_wx
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

add_definitions(
    ${GLEW_DEFINITIONS}
)

set(gboy_wx_hdr
    gbxThread.h
    DisplayDialog.h
    GboyApp.h
    InputDialog.h
    MainFrame.h
    PrecisionTimer.h
    RenderWidget.h
    SoundDialog.h
)

set(gboy_wx_src
    gbxThread.cpp
    DisplayDialog.cpp
    GboyApp.cpp
    InputDialog.cpp
    MainFrame.cpp
    RenderWidget.cpp
    RenderBitmap.cpp
    RenderGL2.cpp
    SoundDialog.cpp
)

WXWIDGETS_ADD_RESOURCES(gboy_wx_src resource.xrc OPTIONS -c -e)

set(gboy_wx_lib
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${wxWidgets_LIBRARIES}
    gboy
)

add_executable(gboy_wx ${MKPROP} ${gboy_wx_src} ${gboy_wx_hdr})
target_link_libraries(gboy_wx ${gboy_wx_lib})

