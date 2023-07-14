local L = {}

L.lexer = "erlang"

L.singleLineComment = "% "

L.extensions = {
	"erl",
	"hrl",
}

L.keywords = {
	[0] = "after and andalso band begin bnot bor bsl bsr bxor case catch cond div end fun if let not of or orelse query receive rem try when xor",
	[1] = "erlang: abs adler32 adler32_combine erlang:append_element apply atom_to_binary atom_to_list binary_to_atom binary_to_existing_atom binary_to_list bitstring_to_list binary_to_term bit_size erlang:bump_reductions byte_size erlang:cancel_timer check_process_code concat_binary crc32 crc32_combine date decode_packet delete_module erlang:demonitor disconnect_node erlang:display element erase erlang:error exit float float_to_list erlang:fun_info erlang:fun_to_list erlang:function_exported garbage_collect get erlang:get_cookie get_keys erlang:get_stacktrace group_leader halt erlang:hash hd erlang:hibernate integer_to_list erlang:integer_to_list iolist_to_binary iolist_size is_alive is_atom is_binary is_bitstring is_boolean erlang:is_builtin is_float is_function is_integer is_list is_number is_pid is_port is_process_alive is_record is_reference is_tuple length link list_to_atom list_to_binary list_to_bitstring list_to_existing_atom list_to_float list_to_integer erlang:list_to_integer list_to_pid list_to_tuple load_module erlang:load_nif erlang:loaded erlang:localtime erlang:localtime_to_universaltime make_ref erlang:make_tuple erlang:max erlang:md5 erlang:md5_final erlang:md5_init erlang:md5_update erlang:memory erlang:min module_loaded erlang:monitor monitor_node node nodes now open_port erlang:phash erlang:phash2 pid_to_list port_close port_command erlang:port_command port_connect port_control erlang:port_call erlang:port_info erlang:port_to_list erlang:ports pre_loaded erlang:process_display process_flag process_info processes purge_module put erlang:raise erlang:read_timer erlang:ref_to_list register registered erlang:resume_process round self erlang:send erlang:send_after erlang:send_nosuspend erlang:set_cookie setelement size spawn spawn_link spawn_monitor spawn_opt split_binary erlang:start_timer statistics erlang:suspend_process erlang:system_flag erlang:system_info erlang:system_monitor erlang:system_profile term_to_binary throw time tl erlang:trace erlang:trace_delivered erlang:trace_info erlang:trace_pattern trunc tuple_size tuple_to_list erlang:universaltime erlang:universaltime_to_localtime unlink unregister whereis erlang:yield",
	[2] = "-define -else -endif -ifdef -ifndef -include -include_lib -undef",
	[3] = "-behavior -behaviour -compile -created -created_by -export -file -import -module -modified -modified_by -record -revision -spec -type -vsn",
	[4] = "@author @clear @copyright @deprecated @doc @docfile @end @equiv @headerfile @hidden @private @reference @see @since @spec @throws @title @todo @TODO @type @version",
	[5] = "@date @docRoot @link @module @package @section @time @type @version",
}

L.styles = {
	["DEFAULT STYLE"] = {
		id = 0,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["DEFAULT COMMENT"] = {
		id = 1,
		fgColor = rgb(0xd00000),
		bgColor = defaultBg,
	},
	["FUNCTION COMMENT"] = {
		id = 14,
		fgColor = rgb(0x3f5fbf),
		bgColor = defaultBg,
	},
	["MODULE COMMENT"] = {
		id = 15,
		fgColor = rgb(0x3f5fbf),
		bgColor = defaultBg,
	},
	["DOCUMENTATION HELPER IN COMMENT"] = {
		id = 16,
		fgColor = rgb(0x3f5fbf),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["DOCUMENTATION MACRO IN COMMENT"] = {
		id = 17,
		fgColor = rgb(0x3f5fbf),
		bgColor = defaultBg,
		fontStyle = 3,
	},
	["VARIABLE"] = {
		id = 2,
		fgColor = rgb(0x7f0000),
		bgColor = defaultBg,
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0x007f00),
		bgColor = defaultBg,
	},
	["STRING"] = {
		id = 5,
		fgColor = rgb(0xff901e),
		bgColor = defaultBg,
	},
	["CHARACTER"] = {
		id = 9,
		fgColor = rgb(0x007f00),
		bgColor = defaultBg,
	},
	["MACRO"] = {
		id = 10,
		fgColor = rgb(0x7f007f),
		bgColor = defaultBg,
	},
	["MACRO QUOTED"] = {
		id = 19,
		fgColor = rgb(0x7f007f),
		bgColor = defaultBg,
	},
	["RECORD"] = {
		id = 11,
		fgColor = rgb(0x7f7f00),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["RECORD QUOTED"] = {
		id = 20,
		fgColor = rgb(0x7f7f00),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["ATOM"] = {
		id = 7,
		fgColor = rgb(0x7f7f00),
		bgColor = defaultBg,
	},
	["ATOM QUOTED"] = {
		id = 18,
		fgColor = rgb(0x7f7f00),
		bgColor = defaultBg,
	},
	["NODE NAME"] = {
		id = 13,
		fgColor = rgb(0x007f7f),
		bgColor = defaultBg,
	},
	["NODE NAME QUOTED"] = {
		id = 21,
		fgColor = rgb(0x007f7f),
		bgColor = defaultBg,
	},
	["RESERVED WORDS"] = {
		id = 4,
		fgColor = rgb(0x00007f),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["BUILT-IN FUNCTIONS"] = {
		id = 22,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["FUNCTION NAME"] = {
		id = 8,
		fgColor = rgb(0x00007f),
		bgColor = defaultBg,
	},
	["MODULE NAME"] = {
		id = 23,
		fgColor = rgb(0x00cccc),
		bgColor = defaultBg,
	},
	["MODULE ATTRIBUTES"] = {
		id = 24,
		fgColor = rgb(0x007f7f),
		bgColor = defaultBg,
	},
	["PREPROCESSOR"] = {
		id = 12,
		fgColor = rgb(0x007f7f),
		bgColor = defaultBg,
	},
	["OPERATORS"] = {
		id = 6,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["UNKNOWN: ERROR"] = {
		id = 31,
		fgColor = defaultFg,
		bgColor = rgb(0xFF0000),
	},
}
return L
