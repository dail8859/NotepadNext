local L = {}

L.lexer = "props"

L.singleLineComment = "# "

L.extensions = {
	"properties",
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
	["SECTION"] = {
		id = 2,
		fgColor = rgb(TypeColor),
		bgColor = rgb(0xF2F4FF),
		fontStyle = 1,
	},
	["ASSIGNMENT"] = {
		id = 3,
		fgColor = rgb(0xFF0000),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["DEFVAL"] = {
		id = 4,
		fgColor = rgb(0xFF0000),
		bgColor = defaultBg,
	},
	["KEY"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 2,
	},
}
return L
