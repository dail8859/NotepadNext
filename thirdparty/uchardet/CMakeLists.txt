file(GLOB UCHARDET_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/LangModels/*.cpp"
)

add_library(uchardet STATIC
    ${UCHARDET_SRC}
)

target_include_directories(uchardet PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)