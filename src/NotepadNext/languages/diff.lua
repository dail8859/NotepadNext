local L = {}

L.lexer = "diff"

L.extensions = {
	"diff",
	"patch",
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
	["COMMAND"] = {
		id = 2,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["HEADER"] = {
		id = 3,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["POSITION"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["DELETED"] = {
		id = 5,
		fgColor = rgb(0x808040),
		bgColor = rgb(0xFFFFFF),
	},
	["ADDED"] = {
		id = 6,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
