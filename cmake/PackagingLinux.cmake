message(STATUS "Configuring Linux packaging")

set(APPDIR ${CMAKE_BINARY_DIR}/AppDir)
set(APPDIR_USR ${APPDIR}/usr)

set(LINUXDEPLOY ${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage)
set(LINUXDEPLOY_QT ${CMAKE_BINARY_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage)

set(APPIMAGE_ENV_VARS
    LDAI_OUTPUT=NotepadNext-v${PROJECT_VERSION}-x86_64.AppImage
)

if(DEFINED ENV{QMAKE} AND NOT "$ENV{QMAKE}" STREQUAL "")
    set(APPIMAGE_QMAKE "$ENV{QMAKE}")
else()
    find_program(APPIMAGE_QMAKE NAMES qmake)
endif()

if(NOT APPIMAGE_QMAKE)
    message(FATAL_ERROR
        "Could not find qmake for AppImage packaging.\n"
        "Please configure with a Qt 6 qmake, for example:\n"
        "  QMAKE=$(which qmake6) cmake -S . -B build -DAPP_DISTRIBUTION=AppImage"
    )
endif()

execute_process(
    COMMAND "${APPIMAGE_QMAKE}" -query QT_VERSION
    OUTPUT_VARIABLE APPIMAGE_QT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    RESULT_VARIABLE APPIMAGE_QMAKE_RESULT
)

if(NOT APPIMAGE_QMAKE_RESULT EQUAL 0 OR NOT APPIMAGE_QT_VERSION MATCHES "^6\\.")
    message(FATAL_ERROR
        "AppImage packaging requires a Qt 6 qmake, but CMake found:\n"
        "  ${APPIMAGE_QMAKE}\n"
        "Reported Qt version:\n"
        "  ${APPIMAGE_QT_VERSION}\n"
        "Please re-run CMake with a Qt 6 qmake, for example:\n"
        "  QMAKE=$(which qmake6) cmake -S . -B build -DAPP_DISTRIBUTION=AppImage"
    )
endif()

message(STATUS "Using qmake for AppImage packaging: ${APPIMAGE_QMAKE}")
list(APPEND APPIMAGE_ENV_VARS QMAKE=${APPIMAGE_QMAKE})

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
        ${APPIMAGE_ENV_VARS}
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
