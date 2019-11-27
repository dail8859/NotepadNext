######## Project settings
cmake_minimum_required(VERSION 2.8.5)
set (PACKAGE_NAME uchardet)
project (${PACKAGE_NAME} CXX C)
enable_testing()

######## Package information
set (PACKAGE_URL https://www.freedesktop.org/wiki/Software/uchardet/)
set (PACKAGE_BUGREPORT https://bugs.freedesktop.org/enter_bug.cgi?product=uchardet)
set (UCHARDET_VERSION_MAJOR 0)
set (UCHARDET_VERSION_MINOR 0)
set (UCHARDET_VERSION_REVISION 6)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	set (version_suffix .debug)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set (
	UCHARDET_VERSION
	${UCHARDET_VERSION_MAJOR}.${UCHARDET_VERSION_MINOR}.${UCHARDET_VERSION_REVISION}${version_suffix}
)

######## Windows

#if (WIN32)
#	set(CMAKE_SHARED_LIBRARY_PREFIX ${CMAKE_INSTALL_PREFIX})
#	set(CMAKE_STATIC_LIBRARY_PREFIX ${CMAKE_INSTALL_PREFIX})
#endif (WIN32)

######## Directory

include(GNUInstallDirs)

######## Configuration

option(BUILD_BINARY "Build executable" ON)
option(BUILD_SHARED_LIBS "Build shared library and link executable to it" ON)

if (BUILD_SHARED_LIBS)
	option(BUILD_STATIC "Build static library" ON)
endif (BUILD_SHARED_LIBS)

configure_file(
	uchardet.pc.in
	uchardet.pc
	@ONLY
)

install(
	FILES
		${CMAKE_BINARY_DIR}/uchardet.pc
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

######## Subdirectories

add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(test)
