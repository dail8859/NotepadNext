local L = {}

L.lexer = "tehex"

L.extensions = {
	"tek",
}

L.keywords = {
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["RECSTART"] = {
		id = 1,
		fgColor = rgb(0x7F0000),
		bgColor = rgb(defaultBg),
	},
	["RECTYPE"] = {
		id = 2,
		fgColor = rgb(0x7F0000),
		bgColor = rgb(defaultBg),
	},
	["RECTYPE_UNKNOWN"] = {
		id = 3,
		fgColor = rgb(0x7F0000),
		bgColor = rgb(defaultBg),
		fontStyle = 2,
	},
	["BYTECOUNT"] = {
		id = 4,
		fgColor = rgb(0x7F7F00),
		bgColor = rgb(defaultBg),
	},
	["BYTECOUNT_WRONG"] = {
		id = 5,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["DATAADDRESS"] = {
		id = 7,
		fgColor = rgb(0x007FFF),
		bgColor = rgb(defaultBg),
	},
	["STARTADDRESS"] = {
		id = 9,
		fgColor = rgb(0x007FFF),
		bgColor = rgb(defaultBg),
	},
	["ADDRESSFIELD_UNKNOWN"] = {
		id = 10,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 2,
	},
	["DATA_ODD"] = {
		id = 12,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["DATA_EVEN"] = {
		id = 13,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["CHECKSUM"] = {
		id = 16,
		fgColor = rgb(0x00BF00),
		bgColor = rgb(defaultBg),
	},
	["CHECKSUM_WRONG"] = {
		id = 17,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["GARBAGE"] = {
		id = 18,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 2,
	},
}
return L
