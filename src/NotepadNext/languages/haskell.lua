local L = {}

L.lexer = "haskell"

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
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 1,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD"] = {
		id = 2,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0xCA6500),
		bgColor = rgb(0xFFFFFF),
	},
	["CHARACTER"] = {
		id = 5,
		fgColor = rgb(0xCA6500),
		bgColor = rgb(0xFFFFFF),
	},
	["CLASS"] = {
		id = 6,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["MODULE"] = {
		id = 7,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["CAPITAL"] = {
		id = 8,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["DATA"] = {
		id = 9,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["IMPORT"] = {
		id = 10,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 11,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTANCE"] = {
		id = 12,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTLINE"] = {
		id = 13,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTBLOCK"] = {
		id = 14,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTBLOCK2"] = {
		id = 15,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTBLOCK3"] = {
		id = 16,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
