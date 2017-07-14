cmake_minimum_required(VERSION 3.2)

project(gui CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories("..")

find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_definitions("-std=c++14")

add_executable(gui main.cpp)

add_subdirectory("../../../gui" "tvm-gui")

target_link_libraries(gui ${Qt5Widgets_LIBRARIES} tvm-gui pthread)
