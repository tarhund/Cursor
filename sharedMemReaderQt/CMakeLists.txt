cmake_minimum_required(VERSION 3.16)
project(camera-viewer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(camera-viewer
    src/main.cpp
    include/cameraviewer.h
)
install(TARGETS camera-viewer RUNTIME DESTINATION bin)
target_include_directories(camera-viewer PRIVATE include)

target_link_libraries(camera-viewer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    rt
)
