local L = {}

L.lexer = "props"

L.extensions = {
	"properties",
}

L.keywords = {
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["SECTION"] = {
		id = 2,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xF2F4FF),
		fontStyle = 1,
	},
	["ASSIGNMENT"] = {
		id = 3,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["DEFVAL"] = {
		id = 4,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEY"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 2,
	},
}
return L
