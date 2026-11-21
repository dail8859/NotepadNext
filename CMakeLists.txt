cmake_minimum_required(VERSION 3.21)

project(NotepadNext
    VERSION 0.15
    DESCRIPTION "Cross-platform text editor"
    LANGUAGES C CXX
)

include(CTest)

option(NOTEPADNEXT_ENABLE_ASAN "Build with AddressSanitizer" OFF)
option(NOTEPADNEXT_WARNINGS_AS_ERRORS "Treat test-target compiler warnings as errors" OFF)

set(APP_DISTRIBUTION "" CACHE STRING "Distribution type")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(APP_COPYRIGHT "Copyright 2019-${CURRENT_YEAR} Justin Dailey")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOTEPADNEXT_ENABLE_ASAN)
    if(MSVC)
        add_compile_options(/fsanitize=address)
        add_link_options(/INCREMENTAL:NO)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag("-fsanitize=address" NOTEPADNEXT_HAS_ASAN)
        if(NOT NOTEPADNEXT_HAS_ASAN)
            message(FATAL_ERROR "AddressSanitizer is not available with this compiler toolchain")
        endif()
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    else()
        message(FATAL_ERROR "AddressSanitizer is not supported by ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

add_compile_definitions(SCI_OWNREGEX)

if(MSVC)
    # https://stackoverflow.com/questions/78598141/first-stdmutexlock-crashes-in-application-built-with-latest-visual-studio
    add_compile_definitions(_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
    add_compile_options(/guard:cf)
    add_link_options(/guard:cf)
endif()

if(UNIX AND NOT APPLE)
    add_compile_options(-fstack-protector-strong -D_FORTIFY_SOURCE=2)
    add_link_options(-Wl,-z,relro -Wl,-z,now)
endif()

find_package(Qt6
    REQUIRED
        COMPONENTS
            Core
            Gui
            Widgets
            Network
            PrintSupport
            Core5Compat
)

set(CMAKE_AUTORCC ON)
qt_standard_project_setup()

set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cpm-cache" CACHE PATH "CPM source cache directory")

add_subdirectory(thirdparty)
add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/Packaging.cmake)
