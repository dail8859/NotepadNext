local L = {}

L.lexer = "ada"

L.singleLineComment = "-- "

L.extensions = {
	"ada",
	"ads",
	"adb",
}

L.keywords = {
	[0] = "abort abs abstract accept access aliased all and array at begin body case constant declare delay delta digits do else elsif end entry exception exit for function generic goto if in interface is limited loop mod new not null of or others out overriding package pragma private procedure protected raise range record rem renames requeue return reverse select separate some subtype synchronized tagged task terminate then type until use when while with xor",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["INSTRUCTION WORD"] = {
		id = 1,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 2,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(defaultBg),
	},
	["DELIMITER"] = {
		id = 4,
		fgColor = rgb(0xFF8080),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["CHARACTER"] = {
		id = 5,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["STRING"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["LABEL"] = {
		id = 9,
		fgColor = rgb(0x804000),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["COMMENT LINE"] = {
		id = 10,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["ILLEGAL"] = {
		id = 11,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(defaultBg),
	},
}
return L
