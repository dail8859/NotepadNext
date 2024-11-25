local L = {}

L.lexer = "yaml"

L.singleLineComment = "# "

L.extensions = {
	"yml",
	"yaml",
}

L.keywords = {
	[0] = "true false yes no",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["IDENTIFIER"] = {
		id = 2,
		fgColor = rgb(OperatorColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["INSTRUCTION WORD"] = {
		id = 3,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8040),
		bgColor = rgb(defaultBg),
	},
	["REFERENCE"] = {
		id = 5,
		fgColor = rgb(0x804000),
		bgColor = rgb(defaultBg),
	},
	["DOCUMENT"] = {
		id = 6,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
	},
	["TEXT"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["ERROR"] = {
		id = 8,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(defaultBg),
	},
}
return L
