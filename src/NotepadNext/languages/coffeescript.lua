local L = {}

L.lexer = "coffeescript"

L.singleLineComment = "# "

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
		bgColor = rgb(defaultBg),
	},
	["DEFAULT"] = {
		id = 11,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["TYPE WORD"] = {
		id = 16,
		fgColor = rgb(TypeColor),
		bgColor = rgb(defaultBg),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(defaultBg),
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(OperatorColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["VERBATIM"] = {
		id = 13,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["REGEX"] = {
		id = 14,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
	["COMMENT DOC KEYWORD"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 18,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
	["PREDEFINED CONSTANT"] = {
		id = 19,
		fgColor = rgb(0x800000),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["COMMENT BLOCK"] = {
		id = 22,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
	["VERBOSE REGEX"] = {
		id = 23,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 3,
	},
	["VERBOSE REGEX COMMENT"] = {
		id = 24,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
}
return L
