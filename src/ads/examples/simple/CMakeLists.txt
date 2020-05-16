cmake_minimum_required(VERSION 3.3)
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
project(ads_example VERSION "1.0") 
set(REQUIRED_QT_VERSION 5.5.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)
set(ads_example_LIBS ${ads_example_LIBS} ${Qt5Core_LIBRARIES})
set(ads_example_INCLUDE ${ads_example_INCLUDE} ${Qt5Core_INCLUDE_DIRS})
set(ads_example_COMPILE_DEFINE ${ads_example_COMPILE_DEFINE} ${Qt5Core_COMPILE_DEFINITIONS} )
set(ads_example_LIBS ${ads_example_LIBS} ${Qt5Gui_LIBRARIES})
set(ads_example_INCLUDE ${ads_example_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
set(ads_example_COMPILE_DEFINE ${ads_example_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})
set(ads_example_LIBS ${ads_example_LIBS} ${Qt5Widgets_LIBRARIES})
set(ads_example_INCLUDE ${ads_example_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
set(ads_example_COMPILE_DEFINE ${ads_example_COMPILE_DEFINE} ${Qt5Widgets_COMPILE_DEFINITIONS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(ads_example_SRCS
    main.cpp
    MainWindow.cpp
    MainWindow.ui
)
add_executable(SimpleExample WIN32 ${ads_example_SRCS})
if(BUILD_STATIC)
    set(ads_example_DEFINE ${ads_example_DEFINE} ADS_STATIC)
endif()
add_dependencies(SimpleExample qtadvanceddocking)
target_include_directories(SimpleExample PUBLIC 
    $<BUILD_INTERFACE:${ads_example_INCLUDE}>
    $<INSTALL_INTERFACE:include>
    )
target_include_directories(SimpleExample PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../src"  ${ads_example_INCLUDE})
target_link_libraries(SimpleExample PRIVATE qtadvanceddocking ${ads_example_LIBS})
target_compile_definitions(SimpleExample PRIVATE ${ads_example_DEFINE})
set_target_properties(SimpleExample PROPERTIES 
    VERSION "1.0"
    SOVERSION 1
    EXPORT_NAME "Qt Advanced Docking System Example"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
