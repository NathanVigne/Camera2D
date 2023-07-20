set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    connectwindow.cpp
    cameramanager.cpp
    gldisplay.cpp
    exportwindow.cpp
    mychart.cpp
    memorymanager.cpp
    workerfit.cpp
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
    workerfit.h
)

set(POJECT_RESSOURCES
    ${PROJECT_SOURCE_DIR}/ressources/ressources.qrc
    shaders.qrc
)

set(app_icon_resource_windows "iconres.rc")


qt_add_executable(Camera2D
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${POJECT_RESSOURCES}
    ${app_icon_resource_windows}
)

target_link_libraries(Camera2D PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::OpenGL
    Qt6::Charts
    GSL::gsl
    GSL::gslcblas
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

add_custom_command(TARGET Camera2D POST_BUILD

    # Adds a post-build event to MyTest  which executes "cmake - E copy_if_different..."
    #                                               this is in-file                                                                    this is out-directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/thorlabs/dlls/Native_64_lib/" $<TARGET_FILE_DIR:Camera2D>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/mightex/dlls/x64/" $<TARGET_FILE_DIR:Camera2D>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/levmar-2.5/BLAS/x64/dlls/" $<TARGET_FILE_DIR:Camera2D>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/levmar-2.5/LAPACK/x64/dlls/" $<TARGET_FILE_DIR:Camera2D>
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/levmar-2.5/mingw64_dll/" $<TARGET_FILE_DIR:Camera2D>
)

qt_finalize_executable(Camera2D)


