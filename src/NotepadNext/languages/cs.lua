local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"cs",
}

L.keywords = {
	[0] = "abstract add alias as ascending async await base break case catch checked continue default delegate descending do dynamic else event explicit extern false finally fixed for foreach from get global goto group if implicit in interface internal into is join let lock namespace new null object operator orderby out override params partial private protected public readonly ref remove return sealed select set sizeof stackalloc switch this throw true try typeof unchecked unsafe using value virtual where while yield",
	[1] = "bool byte char class const decimal double enum float int long sbyte short static string struct uint ulong ushort var void",
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
