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
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["SECTION"] = {
		id = 2,
		fgColor = rgb(cppTypeColor),
		bgColor = rgb(0xF2F4FF),
		fontStyle = 1,
	},
	["ASSIGNMENT"] = {
		id = 3,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["DEFVAL"] = {
		id = 4,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
	},
	["KEY"] = {
		id = 5,
		fgColor = rgb(cppInstructionColor),
		bgColor = darkBg,
		fontStyle = 2,
	},
}
return L
