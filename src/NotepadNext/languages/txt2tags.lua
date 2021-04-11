local L = {}

L.lexer = "txt2tags"

L.extensions = {
	"t2t",
}

L.keywords = {
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["SPECIAL"] = {
		id = 1,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRONG"] = {
		id = 2,
		fgColor = rgb(0x445675),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["STRONG 2 (NOT USED)"] = {
		id = 3,
		fgColor = rgb(0x445675),
		bgColor = rgb(0xFFFFFF),
	},
	["EM1 (ITALIC)"] = {
		id = 4,
		fgColor = rgb(0x653A39),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 2,
	},
	["EM2 (UNDERLINE)"] = {
		id = 5,
		fgColor = rgb(0x386742),
		bgColor = rgb(0xFFFFFF),
	},
	["H1"] = {
		id = 6,
		fgColor = rgb(0xE20700),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["H2"] = {
		id = 7,
		fgColor = rgb(0xE20700),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["H3"] = {
		id = 8,
		fgColor = rgb(0xA81D05),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["H4"] = {
		id = 9,
		fgColor = rgb(0x871704),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["H5"] = {
		id = 10,
		fgColor = rgb(0x871704),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["H6"] = {
		id = 11,
		fgColor = rgb(0x871704),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PRECHAR (NOT USED)"] = {
		id = 12,
		fgColor = rgb(0x871704),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["ULIST"] = {
		id = 13,
		fgColor = rgb(0xE300EE),
		bgColor = rgb(0xFFFFFF),
	},
	["OLIST"] = {
		id = 14,
		fgColor = rgb(0xE300EE),
		bgColor = rgb(0xFFFFFF),
	},
	["BLOCKQUOTE"] = {
		id = 15,
		fgColor = rgb(0x015F52),
		bgColor = rgb(0xFFFFFF),
	},
	["STRIKEOUT"] = {
		id = 16,
		fgColor = rgb(0x644A9B),
		bgColor = rgb(0xFFFFFF),
	},
	["HRULE"] = {
		id = 17,
		fgColor = rgb(0xff901e),
		bgColor = rgb(0xFFFFFF),
	},
	["LINK"] = {
		id = 18,
		fgColor = rgb(0x0930DE),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 2,
	},
	["CODE"] = {
		id = 19,
		fgColor = rgb(0x009f00),
		bgColor = rgb(0xFFFFFF),
	},
	["CODE2"] = {
		id = 20,
		fgColor = rgb(0x009f00),
		bgColor = rgb(0xFFFFFF),
	},
	["CODEBLOCK"] = {
		id = 21,
		fgColor = rgb(0x005f00),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 22,
		fgColor = rgb(0x777777),
		bgColor = rgb(0xFFFFFF),
	},
	["OPTION"] = {
		id = 23,
		fgColor = rgb(0xC0036E),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROC"] = {
		id = 24,
		fgColor = rgb(0x848B00),
		bgColor = rgb(0xFFFFFF),
	},
	["POSTPROC"] = {
		id = 25,
		fgColor = rgb(0xC05600),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
