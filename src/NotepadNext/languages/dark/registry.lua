local L = {}

L.lexer = "registry"

L.singleLineComment = "; "

L.extensions = {
	"reg",
}

L.keywords = {
}

L.styles = {
	["DEFAULT STYLE"] = {
		id = 32,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["DEFAULT"] = {
		id = 0,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = darkBg,
	},
	["VALUE NAME"] = {
		id = 2,
		fgColor = rgb(0x0000FF),
		bgColor = darkBg,
	},
	["STRING"] = {
		id = 3,
		fgColor = rgb(0x7F007F),
		bgColor = darkBg,
	},
	["HEX DIGIT"] = {
		id = 4,
		fgColor = rgb(0x7F0B0C),
		bgColor = darkBg,
	},
	["VALUE TYPE"] = {
		id = 5,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["ADDED KEY"] = {
		id = 6,
		fgColor = rgb(0x530155),
		bgColor = darkBg,
	},
	["REMOVED KEY"] = {
		id = 7,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
	},
	["ESCAPED CHARACTERS IN STRINGS"] = {
		id = 8,
		fgColor = rgb(0x7D8187),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["GUID IN KEY PATH"] = {
		id = 9,
		fgColor = rgb(0x7B5F15),
		bgColor = darkBg,
	},
	["GUID IN STRING"] = {
		id = 10,
		fgColor = rgb(0x7B5F15),
		bgColor = darkBg,
	},
	["PARAMETER"] = {
		id = 11,
		fgColor = rgb(0x0B6561),
		bgColor = darkBg,
	},
	["OPERATORS"] = {
		id = 12,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
}
return L
