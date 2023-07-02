local L = {}

L.lexer = "ihex"

L.extensions = {
	"hex",
}

L.keywords = {
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["RECSTART"] = {
		id = 1,
		fgColor = rgb(0x7F0000),
		bgColor = darkBg,
	},
	["RECTYPE"] = {
		id = 2,
		fgColor = rgb(0x7F0000),
		bgColor = darkBg,
	},
	["RECTYPE_UNKNOWN"] = {
		id = 3,
		fgColor = rgb(0x7F0000),
		bgColor = darkBg,
		fontStyle = 2,
	},
	["BYTECOUNT"] = {
		id = 4,
		fgColor = rgb(0x7F7F00),
		bgColor = darkBg,
	},
	["BYTECOUNT_WRONG"] = {
		id = 5,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["NOADDRESS"] = {
		id = 6,
		fgColor = rgb(0x7F00FF),
		bgColor = darkBg,
	},
	["DATAADDRESS"] = {
		id = 7,
		fgColor = rgb(0x007FFF),
		bgColor = darkBg,
	},
	["STARTADDRESS"] = {
		id = 9,
		fgColor = rgb(0x007FFF),
		bgColor = darkBg,
	},
	["ADDRESSFIELD_UNKNOWN"] = {
		id = 10,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 2,
	},
	["EXTENDEDADDRESS"] = {
		id = 11,
		fgColor = rgb(0x007FFF),
		bgColor = darkBg,
	},
	["DATA_ODD"] = {
		id = 12,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
	["DATA_EVEN"] = {
		id = 13,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["DATA_UNKNOWN"] = {
		id = 14,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 2,
	},
	["DATA_EMPTY"] = {
		id = 15,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["CHECKSUM"] = {
		id = 16,
		fgColor = rgb(0x00BF00),
		bgColor = darkBg,
	},
	["CHECKSUM_WRONG"] = {
		id = 17,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["GARBAGE"] = {
		id = 18,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 2,
	},
}
return L
