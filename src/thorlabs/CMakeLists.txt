add_library(thorlabs STATIC
    tl_camera_sdk_load.c
    thorcam.h
    thorcam.cpp
)
target_link_libraries(thorlabs PRIVATE Qt6::Core)

target_include_directories(thorlabs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

#include_directories(../app)
