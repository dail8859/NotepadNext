if(WIN32)
    include(${CMAKE_CURRENT_LIST_DIR}/PackagingWindows.cmake OPTIONAL)
endif()

if(APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/PackagingMac.cmake OPTIONAL)
endif()

if(UNIX AND NOT APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/PackagingLinux.cmake OPTIONAL)
endif()