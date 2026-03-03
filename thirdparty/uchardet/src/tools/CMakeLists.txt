set(
	UCHARDET_SOURCES
	uchardet.cpp
)

include(CheckSymbolExists)

check_symbol_exists(getopt_long "getopt.h" HAVE_GETOPT_LONG)

# On Windows with MSVC, `getopt_long` is not available by default.
# But some third-party libraries can be used. For example, in `vcpkg`,
# we can find a port named `getopt-win32`.
if (NOT HAVE_GETOPT_LONG)
    find_path(GETOPT_INCLUDE_DIR NAMES getopt.h)
    find_library(GETOPT_LIBRARY NAMES getopt)
endif (NOT HAVE_GETOPT_LONG)

set(UCHARDET_BINARY uchardet)

add_executable(
	${UCHARDET_BINARY}
	${UCHARDET_SOURCES}
)

if (GETOPT_INCLUDE_DIR AND GETOPT_LIBRARY)
    target_include_directories(${UCHARDET_BINARY} PRIVATE ${GETOPT_INCLUDE_DIR})
    target_link_libraries(${UCHARDET_BINARY} PRIVATE ${GETOPT_LIBRARY})
endif (GETOPT_INCLUDE_DIR AND GETOPT_LIBRARY)

target_link_libraries(
	${UCHARDET_BINARY}
	${UCHARDET_LIBRARY}
)

install(
	TARGETS
		${UCHARDET_BINARY}
	EXPORT
		UchardetTargets
	RUNTIME DESTINATION
		${CMAKE_INSTALL_BINDIR}
)
