add_library(mightex STATIC
    mightex.cpp
    mightex.h
)

target_link_libraries(mightex PRIVATE Qt6::Core)

target_include_directories(mightex INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(../app)

