local L = {}

L.lexer = "nimrod"

L.singleLineComment = "! "

L.extensions = {
	"nim",
}

L.keywords = {
	[0] = "addr and as asm block break case cast const continue converter discard div elif else end enum except exception finally for from generic if implies import in include is isnot iterator lambda macro method mod nil not notin object of or out proc ptr raise ref return shl shr template try tuple type var when where while with without xor yield",
}

L.styles = {
	["WHITE SPACE"] = {
		id = 0,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = defaultBg,
	},
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0x007F7F),
		bgColor = defaultBg,
	},
	["STRING"] = {
		id = 3,
		fgColor = rgb(0x7F007F),
		bgColor = defaultBg,
	},
	["SINGLE QUOTED STRING"] = {
		id = 4,
		fgColor = rgb(0x7F007F),
		bgColor = defaultBg,
	},
	["KEYWORD"] = {
		id = 5,
		fgColor = rgb(0x00007F),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["TRIPLE QUOTES"] = {
		id = 6,
		fgColor = rgb(0x7F0000),
		bgColor = defaultBg,
	},
	["TRIPLE DOUBLE QUOTES"] = {
		id = 7,
		fgColor = rgb(0x7F0000),
		bgColor = defaultBg,
	},
	["CLASS NAME DEFINITION"] = {
		id = 8,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["FUNCTION OR METHOD NAME DEFINITION"] = {
		id = 9,
		fgColor = rgb(0x007F7F),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["OPERATORS"] = {
		id = 10,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["IDENTIFIERS"] = {
		id = 11,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["COMMENT-BLOCKS"] = {
		id = 12,
		fgColor = rgb(0x7F7F7F),
		bgColor = defaultBg,
	},
	["END OF LINE WHERE STRING IS NOT CLOSED"] = {
		id = 13,
		fgColor = defaultFg,
		bgColor = rgb(0xE0C0E0),
	},
	["HIGHLIGHTED IDENTIFIERS"] = {
		id = 14,
		fgColor = rgb(0x407090),
		bgColor = defaultBg,
	},
	["DECORATORS"] = {
		id = 15,
		fgColor = rgb(0x805000),
		bgColor = defaultBg,
	},
}
return L
