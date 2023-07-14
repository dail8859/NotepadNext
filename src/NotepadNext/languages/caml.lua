local L = {}

L.lexer = "caml"

L.extensions = {
	"ml",
	"mli",
	"sml",
	"thy",
}

L.keywords = {
	[0] = "and as assert asr begin class constraint do done downto else end exception external false for fun function functor if in include inherit initializer land lazy let lor lsl lsr lxor match method mod module mutable new object of open or private rec sig struct then to true try type val virtual when while with",
	[1] = "option Some None ignore ref lnot succ pred",
	[2] = "array bool char float int list string unit",
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
	["TAGNAME"] = {
		id = 2,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["INSTRUCTION WORD"] = {
		id = 3,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["BUILIN FUNC & TYPE"] = {
		id = 4,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["TYPE"] = {
		id = 5,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["LINENUM"] = {
		id = 6,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["OPERATOR"] = {
		id = 7,
		fgColor = rgb(OperatorColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 8,
		fgColor = rgb(0xFF8000),
		bgColor = defaultBg,
	},
	["CHARACTER"] = {
		id = 9,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["STRING"] = {
		id = 11,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["COMMENT"] = {
		id = 12,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENT LINE"] = {
		id = 13,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENT DOC"] = {
		id = 14,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
}
return L
