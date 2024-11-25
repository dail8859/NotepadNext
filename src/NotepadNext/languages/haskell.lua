local L = {}

L.lexer = "haskell"

L.singleLineComment = "-- "

L.extensions = {
	"hs",
	"lhs",
	"las",
}

L.keywords = {
	[0] = "as case class data default deriving do else hiding if import in infix infixl infixr instance let module newtype of proc qualified rec then type where _",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["IDENTIFIER"] = {
		id = 1,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["KEYWORD"] = {
		id = 2,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0x800080),
		bgColor = rgb(defaultBg),
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0xCA6500),
		bgColor = rgb(defaultBg),
	},
	["CHARACTER"] = {
		id = 5,
		fgColor = rgb(0xCA6500),
		bgColor = rgb(defaultBg),
	},
	["CLASS"] = {
		id = 6,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["MODULE"] = {
		id = 7,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["CAPITAL"] = {
		id = 8,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["DATA"] = {
		id = 9,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["IMPORT"] = {
		id = 10,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["OPERATOR"] = {
		id = 11,
		fgColor = rgb(OperatorColor),
		bgColor = rgb(defaultBg),
	},
	["INSTANCE"] = {
		id = 12,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["COMMENTLINE"] = {
		id = 13,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["COMMENTBLOCK"] = {
		id = 14,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["COMMENTBLOCK2"] = {
		id = 15,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["COMMENTBLOCK3"] = {
		id = 16,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
}
return L
