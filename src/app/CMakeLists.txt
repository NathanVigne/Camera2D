set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    connectwindow.cpp
)

set(PROJECT_HEADERS
    mainwindow.h
    connectwindow.h
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


target_link_libraries(Camera2D PRIVATE Qt6::Widgets)

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
