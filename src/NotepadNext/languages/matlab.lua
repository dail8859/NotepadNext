local L = {}

L.lexer = "matlab"

L.singleLineComment = "% "

L.extensions = {
	"m",
}

L.keywords = {
	[0] = "break case catch classdef continue else elseif end for function global if otherwise parfor persistent return switch try while",
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
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0xFF8000),
		bgColor = defaultBg,
	},
	["INSTRUCTION WORD"] = {
		id = 4,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["STRING"] = {
		id = 5,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["OPERATOR"] = {
		id = 6,
		fgColor = rgb(OperatorColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 7,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["DOUBLE QUOTE STRING"] = {
		id = 8,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
}
return L
