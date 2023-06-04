set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    connectwindow.cpp
    cameramanager.cpp
    gldisplay.cpp
    exportwindow.cpp
    mychart.cpp
    memorymanager.cpp
)

set(PROJECT_HEADERS
    mainwindow.h
    connectwindow.h
    icamera.h
    cameramanager.h
    gldisplay.h
    exportwindow.h
    mychart.h
    memorymanager.h
)

set(POJECT_RESSOURCES
    ${PROJECT_SOURCE_DIR}/ressources/ressources.qrc
    shaders.qrc
)

qt_add_executable(Camera2D
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${POJECT_RESSOURCES}
)

add_custom_command(TARGET Camera2D POST_BUILD

    # Adds a post-build event to MyTest  which executes "cmake - E copy_if_different..."
    #                                               this is in-file                                                                    this is out-directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/thorlabs/dlls/Native_64_lib/" $<TARGET_FILE_DIR:Camera2D>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/mightex/dlls/x64/" $<TARGET_FILE_DIR:Camera2D>
)


target_link_libraries(Camera2D PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::OpenGL
    Qt6::Charts
    thorlabs
    mightex
)

set_target_properties(Camera2D PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Camera2D
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


qt_finalize_executable(Camera2D)


