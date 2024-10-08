local L = {}

L.lexer = "json"

L.first_line = {
	"^{[\r\n]",
}

L.extensions = {
	"json",
}

L.properties = {
	["lexer.json.escape.sequence"] = "1",
	["lexer.json.allow.comments"] = "1",
}

L.keywords = {
	[0] = "false null true",
	[1] = "@id @context @type @value @language @container @list @set @reverse @index @base @vocab @graph",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["NUMBER"] = {
		id = 1,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(defaultBg),
	},
	["STRING"] = {
		id = 2,
		fgColor = rgb(0x800000),
		bgColor = rgb(defaultBg),
	},
	["STRINGEOL"] = {
		id = 3,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["PROPERTYNAME"] = {
		id = 4,
		fgColor = rgb(TypeColor),
		bgColor = rgb(defaultBg),
	},
	["ESCAPESEQUENCE"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["LINECOMMENT"] = {
		id = 6,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["BLOCKCOMMENT"] = {
		id = 7,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["OPERATOR"] = {
		id = 8,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["URI"] = {
		id = 9,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
	},
	["COMPACTIRI"] = {
		id = 10,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["KEYWORD"] = {
		id = 11,
		fgColor = rgb(0x18AF8A),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["LDKEYWORD"] = {
		id = 12,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(defaultBg),
	},
	["ERROR"] = {
		id = 13,
		fgColor = rgb(0xFFFF80),
		bgColor = rgb(0xFF0000),
	},
}
return L
