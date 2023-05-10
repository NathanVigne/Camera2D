add_library(thorlabs STATIC tl_camera_sdk_load.c)

target_include_directories(thorlabs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
