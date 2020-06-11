cmake_minimum_required(VERSION 3.5)
project(ads_demo VERSION ${VERSION_SHORT}) 

find_package(Qt5 5.5 COMPONENTS Core Gui Widgets REQUIRED)
if(WIN32)
    find_package(Qt5 5.5 COMPONENTS AxContainer REQUIRED)
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(ads_demo_SRCS
    main.cpp
    MainWindow.cpp
    mainwindow.ui
    StatusDialog.cpp
    StatusDialog.ui
	demo.qrc
)
add_executable(AdvancedDockingSystemDemo WIN32 ${ads_demo_SRCS})
target_include_directories(AdvancedDockingSystemDemo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(AdvancedDockingSystemDemo PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
if(WIN32)
    target_link_libraries(AdvancedDockingSystemDemo PUBLIC Qt5::AxContainer)
endif()
target_link_libraries(AdvancedDockingSystemDemo PRIVATE qtadvanceddocking)
set_target_properties(AdvancedDockingSystemDemo PROPERTIES 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${VERSION_SHORT}
    EXPORT_NAME "Qt Advanced Docking System Demo"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
#if(BUILD_STATIC)
#    target_compile_definitions(AdvancedDockingSystemDemo PRIVATE ADS_STATIC)
#endif()

