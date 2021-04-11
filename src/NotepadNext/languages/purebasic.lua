local L = {}

L.lexer = "purebasic"

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
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD1"] = {
		id = 3,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 6,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 7,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD2"] = {
		id = 10,
		fgColor = rgb(0x7F7F00),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD3"] = {
		id = 11,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD4"] = {
		id = 12,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["CONSTANT"] = {
		id = 13,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["LABEL"] = {
		id = 15,
		fgColor = rgb(0x7F7F00),
		bgColor = rgb(0xFFFFFF),
	},
	["ERROR"] = {
		id = 16,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["HEXNUMBER"] = {
		id = 17,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["BINNUMBER"] = {
		id = 18,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
