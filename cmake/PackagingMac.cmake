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

install(FILES ${APP_ICON_MACOS}
    DESTINATION NotepadNext.app/Contents/Resources
)

add_custom_target(install_local
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --prefix ${INSTALL_DIR}
    DEPENDS NotepadNext
)

find_program(MACDEPLOYQT_EXECUTABLE macdeployqt REQUIRED)

# Developer ID identity (e.g. "Developer ID Application: Name (TEAMID)") used to
# codesign the bundle with a hardened runtime and secure timestamp so it can be
# notarized. Without this, the dmg is unsigned/ad-hoc signed and Gatekeeper will
# refuse to open it once it has been downloaded (quarantined), reporting it as
# "damaged". See docs/macos-code-signing.md.
set(MACOS_CODESIGN_IDENTITY "" CACHE STRING "Codesigning identity for signing the macOS app bundle for notarization")

set(MACDEPLOYQT_ARGS ${INSTALL_DIR}/NotepadNext.app -dmg)

if(MACOS_CODESIGN_IDENTITY)
    message(STATUS "macOS bundle will be signed for notarization with identity: ${MACOS_CODESIGN_IDENTITY}")
    list(APPEND MACDEPLOYQT_ARGS "-sign-for-notarization=${MACOS_CODESIGN_IDENTITY}")
else()
    message(WARNING "MACOS_CODESIGN_IDENTITY not set: dmg will not be signed for notarization and Gatekeeper will reject it as \"damaged\" once downloaded. See docs/macos-code-signing.md")
endif()

add_custom_target(dmg
    COMMAND ${MACDEPLOYQT_EXECUTABLE}
        ${MACDEPLOYQT_ARGS}
    COMMAND ${CMAKE_COMMAND} -E rename
        ${INSTALL_DIR}/NotepadNext.dmg
        ${CMAKE_BINARY_DIR}/NotepadNext-v${PROJECT_VERSION}.dmg
    DEPENDS install_local
)
