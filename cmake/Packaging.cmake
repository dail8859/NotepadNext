if(WIN32)
    include(${CMAKE_CURRENT_LIST_DIR}/PackagingWindows.cmake OPTIONAL)
endif()

if(APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/PackagingMac.cmake OPTIONAL)
endif()

if(UNIX AND NOT APPLE)
    # Install rules for native Linux packages (rpm/deb via omnipackage). Tagged
    # with a component so `cmake --install --component notepadnext` ships only the
    # app, skipping the bundled ADS library's own install rules (headers/static lib).
    include(GNUInstallDirs)
    install(TARGETS NotepadNext RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT notepadnext)
    install(FILES ${PROJECT_SOURCE_DIR}/deploy/linux/NotepadNext.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications COMPONENT notepadnext)
    install(FILES ${PROJECT_SOURCE_DIR}/deploy/linux/com.github.dail8859.NotepadNext.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo COMPONENT notepadnext)
    install(FILES ${PROJECT_SOURCE_DIR}/icon/NotepadNext.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps COMPONENT notepadnext)
    install(FILES ${PROJECT_SOURCE_DIR}/icon/NotepadNext.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/mimetypes COMPONENT notepadnext)

    # AppImage build machinery (needs qmake) — only when explicitly requested.
    if(APP_DISTRIBUTION)
        include(${CMAKE_CURRENT_LIST_DIR}/PackagingLinux.cmake OPTIONAL)
    endif()
endif()