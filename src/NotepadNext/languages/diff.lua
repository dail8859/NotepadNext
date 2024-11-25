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
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["COMMAND"] = {
		id = 2,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["HEADER"] = {
		id = 3,
		fgColor = rgb(TypeColor),
		bgColor = rgb(defaultBg),
	},
	["POSITION"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(defaultBg),
	},
	["DELETED"] = {
		id = 5,
		fgColor = rgb(0x808040),
		bgColor = rgb(defaultBg),
	},
	["ADDED"] = {
		id = 6,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(defaultBg),
	},
}
return L
