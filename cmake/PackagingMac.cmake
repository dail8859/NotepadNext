message(STATUS "Configuring macOS packaging")

set(INSTALL_DIR ${CMAKE_BINARY_DIR}/install)

# Custom Info.plist
set_target_properties(NotepadNext PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/deploy/macos/info.plist
)

# Application icon
set(APP_ICON_MACOS ${CMAKE_SOURCE_DIR}/icon/NotepadNext.icns)

set_source_files_properties(${APP_ICON_MACOS}
    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
)

target_sources(NotepadNext PRIVATE ${APP_ICON_MACOS})

set_target_properties(NotepadNext PROPERTIES
    MACOSX_BUNDLE_ICON_FILE NotepadNext.icns
)

install(TARGETS NotepadNext
    BUNDLE DESTINATION .
)

add_custom_target(install_local
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --prefix ${INSTALL_DIR}
    DEPENDS NotepadNext
)

find_program(MACDEPLOYQT_EXECUTABLE macdeployqt REQUIRED)

add_custom_target(dmg
    COMMAND ${MACDEPLOYQT_EXECUTABLE}
        ${INSTALL_DIR}/NotepadNext.app
        -dmg
    COMMAND ${CMAKE_COMMAND} -E rename
        ${INSTALL_DIR}/NotepadNext.dmg
        ${CMAKE_BINARY_DIR}/NotepadNext-v${PROJECT_VERSION}.dmg
    DEPENDS install_local
)