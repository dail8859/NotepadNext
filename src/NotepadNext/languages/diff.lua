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
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMAND"] = {
		id = 2,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["HEADER"] = {
		id = 3,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["POSITION"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = defaultBg,
	},
	["DELETED"] = {
		id = 5,
		fgColor = rgb(0x808040),
		bgColor = defaultBg,
	},
	["ADDED"] = {
		id = 6,
		fgColor = rgb(0x0080FF),
		bgColor = defaultBg,
	},
}
return L
