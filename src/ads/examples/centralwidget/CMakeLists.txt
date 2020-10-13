cmake_minimum_required(VERSION 3.5)
project(ads_example_centralwidget VERSION ${VERSION_SHORT}) 
find_package(Qt5 5.5 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_executable(CentralWidgetExample WIN32 
    main.cpp
    mainwindow.cpp
    mainwindow.ui
)
target_include_directories(CentralWidgetExample PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
target_link_libraries(CentralWidgetExample PRIVATE qtadvanceddocking)
target_link_libraries(CentralWidgetExample PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(CentralWidgetExample PROPERTIES 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${VERSION_SHORT}
    EXPORT_NAME "Qt Advanced Docking System Central Widget Example"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
