local L = {}

L.lexer = "objc"

L.singleLineComment = "// "

L.extensions = {
	"mm",
}

L.keywords = {
	[0] = "if else switch case default break goto return for while do continue typedef sizeof NULL self super nil NIL",
	[1] = "interface implementation protocol end private protected public class selector encode defs",
	[2] = "void struct union enum char short int long double float signed unsigned const static extern auto register volatile id Class SEL IMP BOOL",
	[3] = "oneway in out inout bycopy byref",
}

L.styles = {
	["DIRECTIVE"] = {
		id = 19,
		fgColor = rgb(0xA001D6),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["DEFAULT"] = {
		id = 11,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["QUALIFIER"] = {
		id = 20,
		fgColor = rgb(0x95004A),
		bgColor = darkBg,
	},
	["PREPROCESSOR"] = {
		id = 9,
		fgColor = rgb(0x804000),
		bgColor = darkBg,
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["TYPE WORD"] = {
		id = 16,
		fgColor = rgb(TypeColor),
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(OperatorColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["VERBATIM"] = {
		id = 13,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["REGEX"] = {
		id = 14,
		fgColor = darkFg,
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
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
	},
	["COMMENT DOC KEYWORD"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 18,
		fgColor = rgb(0x008080),
		bgColor = darkBg,
	},
}
return L
