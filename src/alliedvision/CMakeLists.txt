add_library(alliedvision STATIC
    alliedvision.cpp
    alliedvision.h
)
target_link_libraries(alliedvision PRIVATE Qt6::Core)

target_include_directories(alliedvision INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(../app)
