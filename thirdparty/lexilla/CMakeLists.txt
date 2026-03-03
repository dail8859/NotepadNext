project(Lexilla CXX)

set(LEXILLA_HEADERS
    include/Lexilla.h
)

# Add all headers from lexlib/
file(GLOB LEXILLA_HEADERS_EXTRA lexlib/*.h)
list(APPEND LEXILLA_HEADERS ${LEXILLA_HEADERS_EXTRA})

set(LEXILLA_SOURCES
    src/Lexilla.cxx
    access/LexillaAccess.cxx
)

file(GLOB LEXILLA_SOURCES_EXTRA lexlib/*.cxx lexers/*.cxx)
list(APPEND LEXILLA_SOURCES ${LEXILLA_SOURCES_EXTRA})

add_library(lexilla STATIC ${LEXILLA_SOURCES} ${LEXILLA_HEADERS})

target_include_directories(lexilla
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/access
        ${CMAKE_CURRENT_SOURCE_DIR}/lexlib
)

if(UNIX)
    target_link_libraries(lexilla PUBLIC dl)
endif()
