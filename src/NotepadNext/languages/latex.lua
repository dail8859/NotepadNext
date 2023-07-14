local L = {}

L.lexer = "latex"

L.singleLineComment = "% "

L.extensions = {
	"tex",
	"sty",
}

L.keywords = {
}

L.styles = {
	["WHITE SPACE"] = {
		id = 0,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["COMMAND"] = {
		id = 1,
		fgColor = rgb(0xAA0000),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["TAG OPENING"] = {
		id = 2,
		fgColor = rgb(0x880088),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["MATH INLINE"] = {
		id = 3,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
	},
	["COMMENT"] = {
		id = 4,
		fgColor = rgb(0x00AA00),
		bgColor = defaultBg,
	},
	["TAG CLOSING"] = {
		id = 5,
		fgColor = rgb(0x880088),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["MATH BLOCK"] = {
		id = 6,
		fgColor = rgb(0x0000AA),
		bgColor = defaultBg,
	},
	["COMMENT BLOCK"] = {
		id = 7,
		fgColor = rgb(0x00AA00),
		bgColor = defaultBg,
	},
	["VERBATIM SEGMENT"] = {
		id = 8,
		fgColor = rgb(0x666666),
		bgColor = defaultBg,
	},
	["SHORT COMMAND"] = {
		id = 9,
		fgColor = rgb(0xAA6600),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["SPECIAL CHAR"] = {
		id = 10,
		fgColor = rgb(0xAAAA00),
		bgColor = defaultBg,
	},
	["COMMAND OPTIONAL ARGUMENT"] = {
		id = 11,
		fgColor = rgb(0xFF9900),
		bgColor = defaultBg,
	},
	["SYNTAX ERROR"] = {
		id = 12,
		fgColor = defaultBg,
		bgColor = rgb(0xff0000),
	},
}
return L
