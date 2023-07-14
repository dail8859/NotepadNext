local L = {}

L.lexer = "makefile"

L.singleLineComment = "# "

L.extensions = {
	"mak",
	"mk",
}

L.keywords = {
}


L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["PREPROCESSOR"] = {
		id = 2,
		fgColor = rgb(OperatorColor),
		bgColor = rgb(0xC0C0C0),
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 3,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
	},
	["OPERATOR"] = {
		id = 4,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["TARGET"] = {
		id = 5,
		fgColor = rgb(0xFF0000),
		bgColor = defaultBg,
	},
	["IDEOL"] = {
		id = 9,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 2,
	},
}

return L
