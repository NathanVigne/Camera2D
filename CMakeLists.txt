cmake_minimum_required(VERSION 3.5)

project(Camera2D VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
#find_package(Qt6 REQUIRED COMPONENTS Concurrent)

add_subdirectory(src/app)
add_subdirectory(src/thorlabs)


