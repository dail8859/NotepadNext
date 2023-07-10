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
		fgColor = rgb(cppTypeColor),
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["INSTRUCTION WORD"] = {
		id = 4,
		fgColor = rgb(cppInstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["STRING"] = {
		id = 5,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 6,
		fgColor = rgb(cppOperatorColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 7,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["DOUBLE QUOTE STRING"] = {
		id = 8,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
}
return L
