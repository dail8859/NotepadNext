local L = {}

L.lexer = "purebasic"

L.singleLineComment = "; "

L.extensions = {
	"pb",
}

L.keywords = {
	[0] = "and break case continue data datasection declare declarecdll declaredll default deftype dim else elseif end enddatasection endenumeration endif endinterface endprocedure endselect endstructure endstructureunion enumeration extends fakereturn for foreach forever global gosub goto if includebinary includefile includepath interface newlist next or procedure procedurecdll proceduredll procedurereturn protected read repeat restore return select shared static step structure structureunion to until wend while xincludefile",
	[1] = "compilercase compilerdefault compilerelse compilerendif compilerendselect compilerif compilerselect",
	[2] = "None",
	[3] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = rgb(defaultBg),
	},
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(defaultBg),
	},
	["KEYWORD1"] = {
		id = 3,
		fgColor = rgb(0x00007F),
		bgColor = rgb(defaultBg),
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(defaultBg),
	},
	["OPERATOR"] = {
		id = 6,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["IDENTIFIER"] = {
		id = 7,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["KEYWORD2"] = {
		id = 10,
		fgColor = rgb(0x7F7F00),
		bgColor = rgb(defaultBg),
	},
	["KEYWORD3"] = {
		id = 11,
		fgColor = rgb(0x00007F),
		bgColor = rgb(defaultBg),
	},
	["KEYWORD4"] = {
		id = 12,
		fgColor = rgb(0x00007F),
		bgColor = rgb(defaultBg),
	},
	["CONSTANT"] = {
		id = 13,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(defaultBg),
	},
	["LABEL"] = {
		id = 15,
		fgColor = rgb(0x7F7F00),
		bgColor = rgb(defaultBg),
	},
	["ERROR"] = {
		id = 16,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["HEXNUMBER"] = {
		id = 17,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(defaultBg),
	},
	["BINNUMBER"] = {
		id = 18,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(defaultBg),
	},
}
return L
