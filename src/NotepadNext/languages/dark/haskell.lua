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
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["IDENTIFIER"] = {
		id = 1,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["KEYWORD"] = {
		id = 2,
		fgColor = rgb(0x0000FF),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0x800080),
		bgColor = darkBg,
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0xCA6500),
		bgColor = darkBg,
	},
	["CHARACTER"] = {
		id = 5,
		fgColor = rgb(0xCA6500),
		bgColor = darkBg,
	},
	["CLASS"] = {
		id = 6,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
	["MODULE"] = {
		id = 7,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["CAPITAL"] = {
		id = 8,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["DATA"] = {
		id = 9,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["IMPORT"] = {
		id = 10,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 11,
		fgColor = rgb(0x000080),
		bgColor = darkBg,
	},
	["INSTANCE"] = {
		id = 12,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENTLINE"] = {
		id = 13,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMENTBLOCK"] = {
		id = 14,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMENTBLOCK2"] = {
		id = 15,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMENTBLOCK3"] = {
		id = 16,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
}
return L
