CPMAddPackage("gh:lua/lua#50c7c915ee2fa239043d5456237f5145d064089b")

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
