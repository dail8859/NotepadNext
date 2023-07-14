local L = {}

L.lexer = "swift"

L.singleLineComment = "// "

L.extensions = {
	"swift",
}

L.keywords = {
	[0] = "class deinit enum extension func import init internal let operator private protocol public static struct subscript typealias var keywordclass.swift.statements=break case continue default do else fallthrough for if in return switch where while",
	[2] = "as dynamicType false is nil self Self super true __COLUMN__ __FILE__ __FUNCTION__ __LINE__",
	[3] = "associativity convenience dynamic didSet final get infix inout lazy left mutating none nonmutating optional override postfix precedence prefix Protocol required right set Type unowned weak willSet",
}

L.styles = {
	["PREPROCESSOR"] = {
		id = 9,
		fgColor = rgb(0x804000),
		bgColor = defaultBg,
	},
	["DEFAULT"] = {
		id = 11,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["TYPE WORD"] = {
		id = 16,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = defaultBg,
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(OperatorColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["VERBATIM"] = {
		id = 13,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["REGEX"] = {
		id = 14,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
	["COMMENT DOC KEYWORD"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 18,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
	["PREPROCESSOR COMMENT"] = {
		id = 23,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["PREPROCESSOR COMMENT DOC"] = {
		id = 24,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
}
return L
