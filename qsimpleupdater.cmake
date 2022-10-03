CPMAddPackage("gh:alex-spataru/QSimpleUpdater#62e0ce7dde4c76e7533f2f0ee53f19ee1690c8dd")

if (QSimpleUpdater_ADDED)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Core Gui Network Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Network Widgets REQUIRED)

    add_library(QSimpleUpdater STATIC
        ${QSimpleUpdater_SOURCE_DIR}/etc/resources/qsimpleupdater.qrc
        ${QSimpleUpdater_SOURCE_DIR}/src/Downloader.ui
        ${QSimpleUpdater_SOURCE_DIR}/src/Downloader.cpp
        ${QSimpleUpdater_SOURCE_DIR}/src/Downloader.h
        ${QSimpleUpdater_SOURCE_DIR}/src/Updater.cpp
        ${QSimpleUpdater_SOURCE_DIR}/src/Updater.h
        ${QSimpleUpdater_SOURCE_DIR}/src/QSimpleUpdater.cpp
        ${QSimpleUpdater_SOURCE_DIR}/include/QSimpleUpdater.h
    )

    target_link_libraries(QSimpleUpdater PRIVATE Qt::Widgets Qt::Network)
    target_include_directories(QSimpleUpdater PUBLIC ${QSimpleUpdater_SOURCE_DIR}/include/)
    target_include_directories(QSimpleUpdater PRIVATE ${QSimpleUpdater_SOURCE_DIR}/src/)
    target_compile_definitions(QSimpleUpdater PRIVATE -DQSU_INCLUDE_MOC)
endif()
