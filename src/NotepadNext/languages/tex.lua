local L = {}

L.lexer = "tex"

L.extensions = {
	"tex",
}

L.keywords = {
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["SPECIAL"] = {
		id = 1,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["GROUP"] = {
		id = 2,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["SYMBOL"] = {
		id = 3,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMAND"] = {
		id = 4,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["TEXT"] = {
		id = 5,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
