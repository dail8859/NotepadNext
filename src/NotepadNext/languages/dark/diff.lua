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
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMAND"] = {
		id = 2,
		fgColor = rgb(InstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["HEADER"] = {
		id = 3,
		fgColor = rgb(TypeColor),
		bgColor = darkBg,
	},
	["POSITION"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["DELETED"] = {
		id = 5,
		fgColor = rgb(0x808040),
		bgColor = darkBg,
	},
	["ADDED"] = {
		id = 6,
		fgColor = rgb(0x0080FF),
		bgColor = darkBg,
	},
}
return L
