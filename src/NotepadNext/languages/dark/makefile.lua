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
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
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
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 4,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
	["TARGET"] = {
		id = 5,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
	},
	["IDEOL"] = {
		id = 9,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 2,
	},
}

return L
