local L = {}

L.lexer = "rust"

L.singleLineComment = "// "

L.extensions = {
	"rs",
}

L.keywords = {
	[0] = "abstract as async become box break const continue crate do dyn else enum extern false final fn for if impl in let loop macro match mod move mut override priv pub ref return self static struct super trait true try type typeof unsafe unsized use virtual where while yield",
	[1] = "bool char f32 f64 i128 i16 i32 i64 i8 isize str u128 u16 u32 u64 u8 usize",
	[2] = "Self",
	[3] = "None",
	[4] = "None",
	[5] = "None",
	[6] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 32,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["WHITESPACE"] = {
		id = 0,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["BLOCK COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = darkBg,
	},
	["LINE COMMENT"] = {
		id = 2,
		fgColor = rgb(0x007F00),
		bgColor = darkBg,
	},
	["BLOCK DOC COMMENT"] = {
		id = 3,
		fgColor = rgb(0x3F703F),
		bgColor = darkBg,
	},
	["LINE DOC COMMENT"] = {
		id = 4,
		fgColor = rgb(0x3F703F),
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 5,
		fgColor = rgb(0x007F7F),
		bgColor = darkBg,
	},
	["KEYWORDS 1"] = {
		id = 6,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["KEYWORDS 2"] = {
		id = 7,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["KEYWORDS 3"] = {
		id = 8,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
	},
	["KEYWORDS 4"] = {
		id = 9,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["KEYWORDS 5"] = {
		id = 10,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["KEYWORDS 6"] = {
		id = 11,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["KEYWORDS 7"] = {
		id = 12,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["REGULAR STRING"] = {
		id = 13,
		fgColor = rgb(0x7F007F),
		bgColor = darkBg,
	},
	["RAW STRING"] = {
		id = 14,
		fgColor = rgb(0xB090B0),
		bgColor = darkBg,
	},
	["CHARACTER"] = {
		id = 15,
		fgColor = rgb(0x7F007F),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 16,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 17,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["LIFETIME"] = {
		id = 18,
		fgColor = rgb(0x007F7F),
		bgColor = darkBg,
	},
	["MACRO"] = {
		id = 19,
		fgColor = rgb(0x7F7F00),
		bgColor = darkBg,
	},
	["LEXICAL ERROR"] = {
		id = 20,
		fgColor = darkFg,
		bgColor = rgb(0xE0C0E0),
	},
	["BYTE STRING"] = {
		id = 21,
		fgColor = rgb(0x7F007F),
		bgColor = darkBg,
	},
	["RAW BYTE STRING"] = {
		id = 22,
		fgColor = rgb(0xB090B0),
		bgColor = darkBg,
	},
	["BYTE CHARACTER"] = {
		id = 23,
		fgColor = rgb(0x7F007F),
		bgColor = darkBg,
	},
}
return L
