message(STATUS "Configuring Linux packaging")

set(APPDIR ${CMAKE_BINARY_DIR}/AppDir)
set(APPDIR_USR ${APPDIR}/usr)

set(LINUXDEPLOY ${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage)
set(LINUXDEPLOY_QT ${CMAKE_BINARY_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage)

install(TARGETS NotepadNext
    RUNTIME DESTINATION bin
)
install(FILES
    ${PROJECT_SOURCE_DIR}/deploy/linux/NotepadNext.desktop
    DESTINATION share/applications
)
install(FILES
    ${PROJECT_SOURCE_DIR}/icon/NotepadNext.svg
    DESTINATION share/icons/hicolor/scalable/apps
)
install(FILES
    ${PROJECT_SOURCE_DIR}/icon/NotepadNext.svg
    DESTINATION share/icons/hicolor/scalable/mimetypes
)

add_custom_target(appdir
    COMMAND ${CMAKE_COMMAND}
        --install .
        --prefix ${APPDIR_USR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS NotepadNext
)

add_custom_target(download_linuxdeploy
    COMMAND wget --no-verbose -O ${LINUXDEPLOY}
        https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    COMMAND wget --no-verbose -O ${LINUXDEPLOY_QT}
        https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    COMMAND chmod +x ${LINUXDEPLOY} ${LINUXDEPLOY_QT}
)

add_custom_target(appimage
    COMMAND ${CMAKE_COMMAND} -E env
        LDAI_OUTPUT=NotepadNext-v${PROJECT_VERSION}-x86_64.AppImage
        ${LINUXDEPLOY}
        --appdir ${APPDIR}
        --executable ${APPDIR_USR}/bin/NotepadNext
        --desktop-file ${APPDIR_USR}/share/applications/NotepadNext.desktop
        --icon-file ${APPDIR_USR}/share/icons/hicolor/scalable/apps/NotepadNext.svg
        --plugin qt
        --output appimage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS appdir download_linuxdeploy
)
