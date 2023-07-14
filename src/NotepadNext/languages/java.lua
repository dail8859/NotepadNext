local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"java",
}

L.properties = {
	["fold.cpp.comment.explicit"] = "0",
	["lexer.cpp.track.preprocessor"] = "0",
	["lexer.cpp.escape.sequence"] = "1",
}

L.keywords = {
	[0] = "instanceof assert if else switch case default break goto return for while do continue new throw throws try catch finally this super extends implements import true false null",
	[1] = "package transient strictfp void char short int long double float const static volatile byte boolean class interface native private protected public final abstract synchronized enum",
	[5] = "FIXME TODO XXX HACK NOTE",
}

L.styles = {
	["WHITE SPACE"] = {
		id = 0,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
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
	["TASK MARKER"] = {
		id = 26,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
		fontStyle = 1,
	},
}
return L
