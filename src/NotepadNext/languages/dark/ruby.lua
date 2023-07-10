local L = {}

L.lexer = "ruby"

L.singleLineComment = "# "

L.extensions = {
	"rb",
	"rbw",
}

L.keywords = {
	[0] = "ARGF ARGV BEGIN END ENV FALSE DATA NIL RUBY_PATCHLEVEL RUBY_PLATFORM RUBY_RELEASE_DATE RUBY_VERSION PLATFORM RELEASE_DATE STDERR STDIN STDOUT TOPLEVEL_BINDING TRUE __ENCODING__ __END__ __FILE__ __LINE__ alias and begin break case class def defined? do else elsif end ensure false for if in module next nil not or redo rescue retry return self super then true undef unless until when while yield",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["ERROR"] = {
		id = 1,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENTLINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["POD"] = {
		id = 3,
		fgColor = rgb(0x004000),
		bgColor = rgb(0xC0FFC0),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["INSTRUCTION"] = {
		id = 5,
		fgColor = rgb(cppInstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808000),
		bgColor = darkBg,
	},
	["CLASS NAME"] = {
		id = 8,
		fgColor = rgb(0x0080C0),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["DEF NAME"] = {
		id = 9,
		fgColor = rgb(0x8080FF),
		bgColor = rgb(0xFFFFCC),
		fontStyle = 1,
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(cppOperatorColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 11,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["REGEX"] = {
		id = 12,
		fgColor = rgb(0x0080FF),
		bgColor = darkBg,
	},
	["GLOBAL"] = {
		id = 13,
		fgColor = rgb(cppOperatorColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["SYMBOL"] = {
		id = 14,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["MODULE NAME"] = {
		id = 15,
		fgColor = rgb(0x804000),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["INSTANCE VAR"] = {
		id = 16,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["CLASS VAR"] = {
		id = 17,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["BACKTICKS"] = {
		id = 18,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xA08080),
	},
	["DATA SECTION"] = {
		id = 19,
		fgColor = rgb(0x600000),
		bgColor = rgb(0xFFF0D8),
	},
	["STRING Q"] = {
		id = 24,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
}
return L
