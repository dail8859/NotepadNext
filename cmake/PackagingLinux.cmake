message(STATUS "Configuring Linux packaging")

set(APPDIR ${CMAKE_BINARY_DIR}/AppDir)
set(APPDIR_USR ${APPDIR}/usr)

set(LINUXDEPLOY ${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage)
set(LINUXDEPLOY_QT ${CMAKE_BINARY_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage)

function(resolve_qt6_qmake out_var)
    set(qt6_qmake_candidates "")

    find_package(Qt6CoreTools QUIET)
    if(TARGET Qt6::qmake)
        get_target_property(qt6_qmake_target Qt6::qmake IMPORTED_LOCATION)
        if(qt6_qmake_target)
            list(APPEND qt6_qmake_candidates "${qt6_qmake_target}")
        endif()
    endif()

    find_program(qmake6_executable
        NAMES qmake6 qmake-qt6 qmake
        HINTS
            /usr/lib/qt6/bin
            /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/qt6/bin
    )
    if(qmake6_executable)
        list(APPEND qt6_qmake_candidates "${qmake6_executable}")
    endif()

    list(REMOVE_DUPLICATES qt6_qmake_candidates)

    foreach(qmake_candidate IN LISTS qt6_qmake_candidates)
        execute_process(
            COMMAND "${qmake_candidate}" -query QT_VERSION
            OUTPUT_VARIABLE qmake_qt_version
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            RESULT_VARIABLE qmake_result
        )
        if(qmake_result EQUAL 0 AND qmake_qt_version MATCHES "^6\\.")
            set(${out_var} "${qmake_candidate}" PARENT_SCOPE)
            return()
        endif()
    endforeach()

    set(${out_var} "" PARENT_SCOPE)
endfunction()

resolve_qt6_qmake(QT6_QMAKE)
if(QT6_QMAKE)
    message(STATUS "Using Qt qmake for AppImage packaging: ${QT6_QMAKE}")
    set(APPIMAGE_ENV_VARS
        LDAI_OUTPUT=NotepadNext-v${PROJECT_VERSION}-x86_64.AppImage
        QMAKE=${QT6_QMAKE}
    )
else()
    message(WARNING "Could not locate a Qt 6 qmake binary for AppImage packaging")
    set(APPIMAGE_ENV_VARS
        LDAI_OUTPUT=NotepadNext-v${PROJECT_VERSION}-x86_64.AppImage
    )
endif()

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
