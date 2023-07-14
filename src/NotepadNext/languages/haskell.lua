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
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["IDENTIFIER"] = {
		id = 1,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["KEYWORD"] = {
		id = 2,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0x800080),
		bgColor = defaultBg,
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0xCA6500),
		bgColor = defaultBg,
	},
	["CHARACTER"] = {
		id = 5,
		fgColor = rgb(0xCA6500),
		bgColor = defaultBg,
	},
	["CLASS"] = {
		id = 6,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["MODULE"] = {
		id = 7,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["CAPITAL"] = {
		id = 8,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["DATA"] = {
		id = 9,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["IMPORT"] = {
		id = 10,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["OPERATOR"] = {
		id = 11,
		fgColor = rgb(OperatorColor),
		bgColor = defaultBg,
	},
	["INSTANCE"] = {
		id = 12,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["COMMENTLINE"] = {
		id = 13,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENTBLOCK"] = {
		id = 14,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENTBLOCK2"] = {
		id = 15,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENTBLOCK3"] = {
		id = 16,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
}
return L
