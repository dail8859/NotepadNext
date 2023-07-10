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
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT LINE"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["GLOBAL"] = {
		id = 5,
		fgColor = rgb(cppInstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["EVENT"] = {
		id = 5,
		fgColor = rgb(cppInstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["ATTRIBUTE"] = {
		id = 16,
		fgColor = rgb(cppTypeColor),
		bgColor = darkBg,
	},
	["CONTROL"] = {
		id = 16,
		fgColor = rgb(cppTypeColor),
		bgColor = darkBg,
	},
	["COMMAND"] = {
		id = 16,
		fgColor = rgb(cppTypeColor),
		bgColor = darkBg,
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(cppOperatorColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
}
return L
