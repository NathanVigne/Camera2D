set(ENV{VCPCK_ROOT} C:/dev/sdks/vcpkg)
set(ENV{VCPKG_DEFAULT_TRIPLET} x64-mingw-static)
set(ENV{GSL_ROOT_DIR} C:/dev/sdks/vcpkg/installed/x64-mingw-static/)

cmake_minimum_required(VERSION 3.5)

include(cmake/app_utils.cmake)
setup_vcpck_before_project()

project(Camera2D VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GSL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets Charts)

add_subdirectory(src/app)
add_subdirectory(src/thorlabs)
add_subdirectory(src/mightex)
add_subdirectory(src/alliedvision)



