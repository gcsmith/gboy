# -----------------------------------------------------------------------------
# Author:  Garrett Smith
# File:    gboy_sdl/CMakeLists.txt
# Created: 03/19/2013
# -----------------------------------------------------------------------------

project(gboy_tests)

find_package(OpenCV REQUIRED)
find_package(SDL2   REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
)

add_executable(camera_test camera_test.cpp)
target_link_libraries(camera_test ${OpenCV_LIBS} ${SDL2_LIBRARY})

