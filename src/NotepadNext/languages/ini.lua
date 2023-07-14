local L = {}

L.lexer = "props"

L.singleLineComment = "; "

L.first_line = {
	"^%[.+%][\r\n]",
}

L.extensions = {
	"ini",
	"inf",
	"url",
	"wer",
}

L.keywords = {
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
	["SECTION"] = {
		id = 2,
		fgColor = rgb(TypeColor),
		bgColor = rgb(0xF2F4FF),
		fontStyle = 9,
	},
	["ASSIGNMENT"] = {
		id = 3,
		fgColor = rgb(0xFF0000),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["DEFVAL"] = {
		id = 4,
		fgColor = rgb(0xFF0000),
		bgColor = defaultBg,
	},
	["KEY"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 2,
	},
}
return L
