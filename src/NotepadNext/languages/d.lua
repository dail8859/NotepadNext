local L = {}

L.lexer = "d"

L.singleLineComment = "// "

L.extensions = {
	"d",
}

L.keywords = {
	[0] = "abstract alias align asm assert auto body bool break byte case cast catch cdouble cent cfloat char class const continue creal dchar debug default delegate delete deprecated do double else enum export extern false final finally float for foreach foreach_reverse function goto idouble if ifloat immutable import in inout int interface invariant ireal is lazy long mixin module new nothrow null out override package pragma private protected public pure real ref return scope shared short static struct super switch synchronized template this throw true try typedef typeid typeof ubyte ucent uint ulong union unittest ushort version void volatile wchar while with",
	[1] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["IDENTIFIER"] = {
		id = 14,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["INSTRUCTION WORD"] = {
		id = 6,
		fgColor = rgb(InstructionColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["KEWORD1"] = {
		id = 7,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["KEWORD2"] = {
		id = 8,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["KEWORD3"] = {
		id = 9,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["KEWORD4"] = {
		id = 20,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["KEWORD5"] = {
		id = 21,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["KEWORD6"] = {
		id = 22,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["NUMBER"] = {
		id = 5,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(defaultBg),
	},
	["STRING"] = {
		id = 10,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["CHARACTER"] = {
		id = 12,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["OPERATOR"] = {
		id = 13,
		fgColor = rgb(OperatorColor),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
	["COMMENT NESTED"] = {
		id = 4,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
	["COMMENT DOC KEYWORD"] = {
		id = 16,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = rgb(defaultBg),
	},
	["STRING B"] = {
		id = 18,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["STRING R"] = {
		id = 19,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
}
return L
