local L = {}

L.lexer = "coffeescript"

L.extensions = {
	"coffee",
	"litcoffee",
}

L.keywords = {
	[0] = "new instanceof do return break continue throw for while until loop if else unless switch when then in of by typeof delete where try catch finally prototype jQuery case default function var void with const let enum export import native __hasProp __extends __slice __bind __indexOf",
	[1] = "Object Number Boolean Array String RegExp Function Date Math eval setInterval clearInterval setTimeout clearTimeout isFinite isNaN parseFloat parseInt escape unescape console encodeURI encodeURIComponent decodeURI decodeURIComponent window document navigator location history screen alert prompt process GLOBAL require exports",
	[3] = "false true yes no on off undefined null NaN Infinity",
}

L.styles = {
	["PREPROCESSOR"] = {
		id = 9,
		fgColor = rgb(0x804000),
		bgColor = rgb(0xFFFFFF),
	},
	["DEFAULT"] = {
		id = 11,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["TYPE WORD"] = {
		id = 16,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["VERBATIM"] = {
		id = 13,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["REGEX"] = {
		id = 14,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC KEYWORD"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 18,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["PREDEFINED CONSTANT"] = {
		id = 19,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT BLOCK"] = {
		id = 22,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["VERBOSE REGEX"] = {
		id = 23,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 3,
	},
	["VERBOSE REGEX COMMENT"] = {
		id = 24,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
