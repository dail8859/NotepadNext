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
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["DEFAULT"] = {
		id = 0,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = defaultBg,
	},
	["VALUE NAME"] = {
		id = 2,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
	},
	["STRING"] = {
		id = 3,
		fgColor = rgb(0x7F007F),
		bgColor = defaultBg,
	},
	["HEX DIGIT"] = {
		id = 4,
		fgColor = rgb(0x7F0B0C),
		bgColor = defaultBg,
	},
	["VALUE TYPE"] = {
		id = 5,
		fgColor = rgb(0x00007F),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["ADDED KEY"] = {
		id = 6,
		fgColor = rgb(0x530155),
		bgColor = defaultBg,
	},
	["REMOVED KEY"] = {
		id = 7,
		fgColor = rgb(0xFF0000),
		bgColor = defaultBg,
	},
	["ESCAPED CHARACTERS IN STRINGS"] = {
		id = 8,
		fgColor = rgb(0x7D8187),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["GUID IN KEY PATH"] = {
		id = 9,
		fgColor = rgb(0x7B5F15),
		bgColor = defaultBg,
	},
	["GUID IN STRING"] = {
		id = 10,
		fgColor = rgb(0x7B5F15),
		bgColor = defaultBg,
	},
	["PARAMETER"] = {
		id = 11,
		fgColor = rgb(0x0B6561),
		bgColor = defaultBg,
	},
	["OPERATORS"] = {
		id = 12,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
}
return L
