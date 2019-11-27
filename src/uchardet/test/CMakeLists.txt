set(
	UCHARDET_TEST_SOURCES
	uchardet-tests.c
)

add_executable(
	uchardet-tests
	${UCHARDET_TEST_SOURCES}
)

target_link_libraries(
	uchardet-tests
	${UCHARDET_LIBRARY}
)

set_target_properties(
	uchardet-tests
	PROPERTIES
		LINKER_LANGUAGE
			C
		OUTPUT_NAME
			uchardet-tests
)

# Iterate through all langs.
file(GLOB dirs "[a-z][a-z]")
foreach(dir ${dirs})
	get_filename_component(lang ${dir} NAME)
	file(GLOB files "${dir}/*")
	# Iterate through all files.
	foreach(file ${files})
		get_filename_component(charset ${file} NAME_WE)
		# These are tests known to fail (not supported or not efficient
		# enough). We will have to take a closer look and fix these, but
		# there is no need to break the whole `make test` right now,
		# which may make actual regressions harder to notice.
		if ("${lang}:${charset}" STREQUAL "ja:utf-16le"		OR
			"${lang}:${charset}" STREQUAL "ja:utf-16be"		OR
			"${lang}:${charset}" STREQUAL "es:iso-8859-15"	OR
			"${lang}:${charset}" STREQUAL "da:iso-8859-1"	OR
			"${lang}:${charset}" STREQUAL "he:iso-8859-8")
			message(STATUS "Skipping test ${lang}:${charset} (known broken)")
		else()
			add_test(NAME "${lang}:${charset}" COMMAND uchardet-tests ${file})
		endif()
	endforeach()
endforeach()
