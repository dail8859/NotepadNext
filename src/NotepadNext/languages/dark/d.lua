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
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["IDENTIFIER"] = {
		id = 14,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["INSTRUCTION WORD"] = {
		id = 6,
		fgColor = rgb(InstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["KEWORD1"] = {
		id = 7,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["KEWORD2"] = {
		id = 8,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["KEWORD3"] = {
		id = 9,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["KEWORD4"] = {
		id = 20,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["KEWORD5"] = {
		id = 21,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["KEWORD6"] = {
		id = 22,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 5,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["STRING"] = {
		id = 10,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["CHARACTER"] = {
		id = 12,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 13,
		fgColor = rgb(OperatorColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
	},
	["COMMENT NESTED"] = {
		id = 4,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
	},
	["COMMENT DOC KEYWORD"] = {
		id = 16,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
	},
	["STRING B"] = {
		id = 18,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["STRING R"] = {
		id = 19,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
}
return L
