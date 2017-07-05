cmake_minimum_required(VERSION 3.2)

project(tvm-gui CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories("../include")

find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

file(GLOB_RECURSE CPPS ./src/*.cpp)
file(GLOB_RECURSE HEADERS ./src/*.h)

add_definitions("-std=c++14")
add_definitions("-DNODE_EDITOR_SHARED")

add_library(tvm-gui SHARED ${CPPS})

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("nodeeditor")

target_link_libraries(tvm-gui ${Qt5Widgets_LIBRARIES} nodes pthread)

target_include_directories(tvm-gui INTERFACE "../include")
target_include_directories(tvm-gui INTERFACE "include")

install(TARGETS tvm-gui DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/tvm/gui)
