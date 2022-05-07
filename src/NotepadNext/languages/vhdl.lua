local L = {}

L.lexer = "vhdl"

L.singleLineComment = "-- "

L.extensions = {
	"vhd",
	"vhdl",
}

L.keywords = {
	[0] = "access after alias all architecture array assert attribute begin block body buffer bus case case? converse component configuration constant designated_subtype disconnect downto else elsif end entity exit file for force function generate generic group guarded if impure in index inertial inout is label library linkage literal loop map new next null of on open others out package port postponed procedure process pure range record register reject report return select severity shared signal subtype then to transport type unaffected units until use variable wait when while with assume assume_guarantee context cover default fairness force parameter privat property protect protected record reflect release restrict restrict_guarantee sequence strong value view vmode vprop vunit",
	[1] = "abs and mod nand nor not or rem rol ror sla sll sra srl xnor xor",
	[2] = "left right low high ascending image value pos val succ pred leftof rightof base range reverse_range length delayed stable quiet transaction event active last_event last_active last_value driving driving_value simple_name path_name instance_name minimum maximum",
	[3] = "now readline read writeline write endfile resolved to_bit to_bitvector to_stdulogic to_stdlogicvector to_stdulogicvector to_x01 to_x01z to_UX01 rising_edge falling_edge is_x shift_left shift_right rotate_left rotate_right resize to_integer to_unsigned to_signed to_string to_bstring to_binarystring to_ostring to_octalstring to_hstring to_hexstring std_match to_01",
	[4] = "std ieee work standard textio std_logic_1164 std_logic_arith std_logic_misc std_logic_signed std_logic_textio std_logic_unsigned numeric_bit numeric_std math_complex math_real vital_primitives vital_timing",
	[5] = "boolean boolean_vector bit character severity_level integer integer_vector real real_vector time time_vector delay_length natural positive string bit_vector file_open_kind file_open_status line text side width std_ulogic std_ulogic_vector std_logic std_logic_vector X01 X01Z UX01 UX01Z unsigned signed",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LIne"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE BANG"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 5,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 6,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTRUCTION"] = {
		id = 8,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["STD OPERATOR"] = {
		id = 9,
		fgColor = rgb(0x0080C0),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["ATTRIBUTE"] = {
		id = 10,
		fgColor = rgb(0x8080FF),
		bgColor = rgb(0xFFFFCC),
		fontStyle = 1,
	},
	["STD FUNCTION"] = {
		id = 11,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["STD PACKAGE"] = {
		id = 12,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
	},
	["STD TYPE"] = {
		id = 13,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["USER DEFINE"] = {
		id = 14,
		fgColor = rgb(0xB5E71F),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
