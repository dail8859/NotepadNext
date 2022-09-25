CPMAddPackage("gh:lua/lua#26be27459b11feabed52cf40aaa76f86c7edc977")

if (lua_ADDED)
    set(CMAKE_C_STANDARD 99)
    FILE(GLOB lua_sources ${lua_SOURCE_DIR}/*.c)
    list(REMOVE_ITEM lua_sources "${lua_SOURCE_DIR}/lua.c" "${lua_SOURCE_DIR}/luac.c" "${lua_SOURCE_DIR}/onelua.c")
    add_library(lua STATIC ${lua_sources})
    target_include_directories(lua PUBLIC $<BUILD_INTERFACE:${lua_SOURCE_DIR}>)
    #target_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

    if (WINDOWS)
        target_compile_definitions(lua PRIVATE "-DLUA_USE_WINDOWS")
    endif()

    target_compile_definitions(lua PRIVATE "-DLUA_COMPAT_5_2")
endif()
