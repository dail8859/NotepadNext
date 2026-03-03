set(
	UCHARDET_SYMBOLS
	uchardet_new
	uchardet_delete
	uchardet_handle_data
	uchardet_data_end
	uchardet_reset
	uchardet_get_charset
)

set (LINK_FLAGS "")

if (APPLE)
	# Create a symbols_list file for the Darwin linker.
	string(REPLACE ";" "\n_" _symbols "${UCHARDET_SYMBOLS}")
	set(_symbols_list "${CMAKE_CURRENT_BINARY_DIR}/symbols.list")
	file(WRITE ${_symbols_list} "_${_symbols}\n")

	set(LINK_FLAGS
		"${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	# Create a version script for the GNU ld.
	set(_symbols "{ global: ${UCHARDET_SYMBOLS}; local: *; };")
	set(_version_script "${CMAKE_CURRENT_BINARY_DIR}/version.script")
	file(WRITE ${_version_script} "${_symbols}\n")

	set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set_target_properties(
	${UCHARDET_LIBRARY}
	PROPERTIES
		LINK_FLAGS
			"${LINK_FLAGS}"
)
