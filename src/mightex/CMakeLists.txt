## Includes files
#INCLUDE_DIRECTORIES(/dlls/x64)

## Link libraries
#LINK_DIRECTORIES(/dlls/x64)


add_library(mightex STATIC
    mightex.cpp
    mightex.h
    MT_USBCamera_SDK.h
)

target_link_libraries(mightex PRIVATE Qt6::Core C:/dev/Camera2D/Camera2D/src/mightex/dlls/x64/MT_USBCamera_SDK.lib)

target_include_directories(mightex INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(../app)

