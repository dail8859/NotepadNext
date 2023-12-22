local L = {}

L.lexer = "tex"

L.singleLineComment = "% "

L.extensions = {
	"tex",
}

L.keywords = {
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["SPECIAL"] = {
		id = 1,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["GROUP"] = {
		id = 2,
		fgColor = rgb(TypeColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["SYMBOL"] = {
		id = 3,
		fgColor = rgb(0x800000),
		bgColor = rgb(defaultBg),
	},
	["COMMAND"] = {
		id = 4,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["TEXT"] = {
		id = 5,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
}
return L
