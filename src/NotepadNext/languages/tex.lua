local L = {}

L.lexer = "tex"

L.singleLineComment = "% "

L.extensions = {
	"tex",
}

L.keywords = {
}

defaultBg=rgb(0x202020)
L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = defaultBg,
		bgColor = defaultBg,
	},
	["SPECIAL"] = {
		id = 1,
		fgColor = rgb(0xFF8000),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["GROUP"] = {
		id = 2,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["SYMBOL"] = {
		id = 3,
		fgColor = rgb(0x800000),
		bgColor = defaultBg,
	},
	["COMMAND"] = {
		id = 4,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["TEXT"] = {
		id = 5,
		fgColor = defaultBg,
		bgColor = defaultBg,
	},
}
return L
