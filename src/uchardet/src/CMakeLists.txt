set(
	UCHARDET_HEADERS
	uchardet.h
)

set(
	UCHARDET_SOURCES
	CharDistribution.cpp
	JpCntx.cpp
	LangModels/LangArabicModel.cpp
	LangModels/LangBulgarianModel.cpp
	LangModels/LangRussianModel.cpp
	LangModels/LangEsperantoModel.cpp
	LangModels/LangFrenchModel.cpp
	LangModels/LangDanishModel.cpp
	LangModels/LangGermanModel.cpp
	LangModels/LangGreekModel.cpp
	LangModels/LangHungarianModel.cpp
	LangModels/LangHebrewModel.cpp
	LangModels/LangSpanishModel.cpp
	LangModels/LangThaiModel.cpp
	LangModels/LangTurkishModel.cpp
	LangModels/LangVietnameseModel.cpp
	nsHebrewProber.cpp
	nsCharSetProber.cpp
	nsBig5Prober.cpp
	nsEUCJPProber.cpp
	nsEUCKRProber.cpp
	nsEUCTWProber.cpp
	nsEscCharsetProber.cpp
	nsEscSM.cpp
	nsGB2312Prober.cpp
	nsMBCSGroupProber.cpp
	nsMBCSSM.cpp
	nsSBCSGroupProber.cpp
	nsSBCharSetProber.cpp
	nsSJISProber.cpp
	nsUTF8Prober.cpp
	nsLatin1Prober.cpp
	nsUniversalDetector.cpp
	uchardet.cpp
)

set (UCHARDET_LIBRARY libuchardet)
set (UCHARDET_LIBRARY libuchardet PARENT_SCOPE)

if (BUILD_STATIC AND BUILD_SHARED_LIBS)
	set (UCHARDET_STATIC_LIBRARY libuchardet_static)
endif ()

add_definitions(
	-DVERSION="${UCHARDET_VERSION}"
	-Wall
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(
		-O0
		-g3
	)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

add_library(
	${UCHARDET_LIBRARY}
	${UCHARDET_SOURCES}
)

if (UCHARDET_STATIC_LIBRARY)
	add_library(
		${UCHARDET_STATIC_LIBRARY}
		STATIC
		${UCHARDET_SOURCES}
	)
endif (UCHARDET_STATIC_LIBRARY)

set_target_properties(
	${UCHARDET_LIBRARY}
	PROPERTIES
		LINKER_LANGUAGE
			CXX
		OUTPUT_NAME
			${PACKAGE_NAME}
		VERSION
			${UCHARDET_VERSION}
		SOVERSION
			${UCHARDET_VERSION_MAJOR}
)

if (UCHARDET_STATIC_LIBRARY)
	set_target_properties(
		${UCHARDET_STATIC_LIBRARY}
		PROPERTIES
			LINKER_LANGUAGE
				CXX
			OUTPUT_NAME
				${PACKAGE_NAME}
	)
endif (UCHARDET_STATIC_LIBRARY)

if (NOT WIN32)
	install(
		TARGETS
			${UCHARDET_LIBRARY}
		LIBRARY DESTINATION
			${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION
			${CMAKE_INSTALL_LIBDIR}
	)
else (NOT WIN32)
	install(
		TARGETS
			${UCHARDET_LIBRARY}
		RUNTIME DESTINATION
			${CMAKE_INSTALL_BINDIR}
		ARCHIVE DESTINATION
			${CMAKE_INSTALL_LIBDIR}
	)
endif (NOT WIN32)

if (UCHARDET_STATIC_LIBRARY)
	install(
		TARGETS
			${UCHARDET_STATIC_LIBRARY}
		ARCHIVE DESTINATION
			${CMAKE_INSTALL_LIBDIR}
	)
endif (UCHARDET_STATIC_LIBRARY)

install(
	FILES
		${UCHARDET_HEADERS}
	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}/${PACKAGE_NAME}
)

include(symbols.cmake)

if (BUILD_BINARY)
	add_subdirectory(tools)
endif (BUILD_BINARY)
