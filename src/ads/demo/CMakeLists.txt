cmake_minimum_required(VERSION 3.3)
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
project(ads_demo VERSION "1.0") 
set(REQUIRED_QT_VERSION 5.5.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)
set(ads_demo_LIBS ${ads_demo_LIBS} ${Qt5Core_LIBRARIES})
set(ads_demo_INCLUDE ${ads_demo_INCLUDE} ${Qt5Core_INCLUDE_DIRS})
set(ads_demo_COMPILE_DEFINE ${ads_demo_COMPILE_DEFINE} ${Qt5Core_COMPILE_DEFINITIONS} )
set(ads_demo_LIBS ${ads_demo_LIBS} ${Qt5Gui_LIBRARIES})
set(ads_demo_INCLUDE ${ads_demo_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
set(ads_demo_COMPILE_DEFINE ${ads_demo_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})
set(ads_demo_LIBS ${ads_demo_LIBS} ${Qt5Widgets_LIBRARIES})
set(ads_demo_INCLUDE ${ads_demo_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
set(ads_demo_COMPILE_DEFINE ${ads_demo_COMPILE_DEFINE} ${Qt5Widgets_COMPILE_DEFINITIONS})
if(WIN32)
	find_package(Qt5AxContainer ${REQUIRED_QT_VERSION} REQUIRED)
	set(ads_demo_LIBS ${ads_demo_LIBS} ${Qt5AxContainer_LIBRARIES})
	set(ads_demo_INCLUDE ${ads_demo_INCLUDE} ${Qt5AxContainer_INCLUDE_DIRS})
	set(ads_demo_COMPILE_DEFINE ${ads_demo_COMPILE_DEFINE} ${Qt5AxContainer_COMPILE_DEFINITIONS})
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
if(BUILD_STATIC)
    set(ads_demo_DEFINE ${ads_demo_DEFINE} ADS_STATIC)
endif()
add_dependencies(AdvancedDockingSystemDemo qtadvanceddocking)
target_include_directories(AdvancedDockingSystemDemo PUBLIC 
    $<BUILD_INTERFACE:${ads_demo_INCLUDE}>
    $<INSTALL_INTERFACE:include>
    )
target_include_directories(AdvancedDockingSystemDemo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src"  ${ads_demo_INCLUDE})
target_link_libraries(AdvancedDockingSystemDemo PRIVATE qtadvanceddocking ${ads_demo_LIBS})
target_compile_definitions(AdvancedDockingSystemDemo PRIVATE ${ads_demo_DEFINE})
set_target_properties(AdvancedDockingSystemDemo PROPERTIES 
    VERSION "1.0"
    SOVERSION 1
    EXPORT_NAME "Qt Advanced Docking System Demo"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
