local L = {}

L.lexer = "smalltalk"

L.extensions = {
	"st",
}

L.keywords = {
	[0] = "ifTrue: ifFalse: whileTrue: whileFalse: ifNil: ifNotNil: whileTrue whileFalse repeat isNil notNil",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 1,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 3,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["SYMBOL"] = {
		id = 4,
		fgColor = rgb(0x408080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["BINARY"] = {
		id = 5,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
	},
	["BOOL"] = {
		id = 6,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["SELF"] = {
		id = 7,
		fgColor = rgb(0x8080FF),
		bgColor = rgb(0xFFFFCC),
		fontStyle = 1,
	},
	["SUPER"] = {
		id = 8,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xECFFEA),
		fontStyle = 1,
	},
	["NIL"] = {
		id = 9,
		fgColor = rgb(0x8080C0),
		bgColor = rgb(0xFFFFFF),
	},
	["GLOBAL"] = {
		id = 10,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
	},
	["RETURN"] = {
		id = 11,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["SPECIAL"] = {
		id = 12,
		fgColor = rgb(0x808000),
		bgColor = rgb(0xFFFFFF),
	},
	["KWS END"] = {
		id = 13,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xFFFFFF),
	},
	["ASSIGN"] = {
		id = 14,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFFFF),
	},
	["CHARACTER"] = {
		id = 15,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["SPECIAL SELECTOR"] = {
		id = 16,
		fgColor = rgb(0xFF80C0),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
