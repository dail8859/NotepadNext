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
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
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
		bgColor = rgb(defaultBg),
	},
	["OPERATOR"] = {
		id = 4,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["TARGET"] = {
		id = 5,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(defaultBg),
	},
	["IDEOL"] = {
		id = 9,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 2,
	},
}

return L
