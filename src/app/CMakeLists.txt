set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    connectwindow.cpp
    #tl_camera_sdk_load.c

)

set(PROJECT_HEADERS
    mainwindow.h
    connectwindow.h
    #tl_camera_sdk.h
    #tl_camera_sdk_load.h
)

set(PROJECT_UI
    mainwindow.ui
)

set(POJECT_RESSOURCES
    ${PROJECT_SOURCE_DIR}/ressources/ressources.qrc
)

qt_add_executable(Camera2D
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_UI}
    ${POJECT_RESSOURCES}
)

add_custom_command(TARGET Camera2D POST_BUILD

    # Adds a post-build event to MyTest  which executes "cmake - E copy_if_different..."
    #                                               this is in-file                                                                    this is out-directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/thorlabs/dlls/Native_64_lib/" $<TARGET_FILE_DIR:Camera2D>

)

                       #
target_link_libraries(Camera2D PRIVATE
    Qt6::Widgets
    #Qt6::Concurrent
    thorlabs
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


