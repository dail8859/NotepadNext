local L = {}

L.lexer = "pascal"

L.singleLineComment = "// "

L.extensions = {
	"pas",
	"pp",
	"p",
	"inc",
	"lpr",
}

L.keywords = {
	[0] = "and array asm begin case cdecl class const constructor default destructor div do downto else end end. except exit exports external far file finalization finally for function goto if implementation in index inherited initialization inline interface label library message mod near nil not object of on or out overload override packed pascal private procedure program property protected public published raise read record register repeat resourcestring safecall set shl shr stdcall stored string then threadvar to try type unit until uses var virtual while with write xor",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["IDENTIFIER"] = {
		id = 1,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMENT LINE"] = {
		id = 3,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMENT DOC"] = {
		id = 4,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
	},
	["PREPROCESSOR"] = {
		id = 5,
		fgColor = rgb(0x804000),
		bgColor = darkBg,
	},
	["PREPROCESSOR2"] = {
		id = 6,
		fgColor = rgb(0x804000),
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 7,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["HEX NUMBER"] = {
		id = 8,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["INSTRUCTION WORD"] = {
		id = 9,
		fgColor = rgb(0x0000FF),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["STRING"] = {
		id = 10,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["CHARACTER"] = {
		id = 12,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 13,
		fgColor = rgb(0x000080),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["ASM"] = {
		id = 14,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
}
return L
