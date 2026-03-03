message(STATUS "Configuring macOS packaging")

set(INSTALL_DIR ${CMAKE_BINARY_DIR}/install)

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