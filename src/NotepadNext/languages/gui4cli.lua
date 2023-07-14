local L = {}

L.lexer = "gui4cli"

L.singleLineComment = "// "

L.keywords = {
	[0] = "None",
	[1] = "None",
	[2] = "None",
	[3] = "None",
	[4] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["COMMENT LINE"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENT"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["GLOBAL"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["EVENT"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["ATTRIBUTE"] = {
		id = 16,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["CONTROL"] = {
		id = 16,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["COMMAND"] = {
		id = 16,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(OperatorColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
}
return L
