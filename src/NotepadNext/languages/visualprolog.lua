local L = {}

L.lexer = "visualprolog"

L.singleLineComment = "% "

L.extensions = {
	"pro",
	"cl",
	"i",
	"pack",
	"ph",
}

L.keywords = {
	[0] = "goal namespace interface class implement open inherits supports resolve delegate monitor constants domains predicates constructors properties clauses facts",
	[1] = "guard language stdcall apicall c thiscall prolog digits if then elseif else foreach do try catch finally erroneous failure procedure determ multi nondeterm anyflow and or externally from div mod rem quot in orelse otherwise",
	[2] = "include bininclude requires orrequires if then else elseif endif error message export externally options",
	[3] = "short detail end exception withdomain",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["MAJOR"] = {
		id = 1,
		fgColor = rgb(0x808000),
		bgColor = rgb(defaultBg),
	},
	["MINOR"] = {
		id = 2,
		fgColor = rgb(0x333399),
		bgColor = rgb(defaultBg),
	},
	["DIRECTIVE"] = {
		id = 3,
		fgColor = rgb(0x800080),
		bgColor = rgb(defaultBg),
	},
	["COMMENT BLOCK"] = {
		id = 4,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["COMMENT LINE"] = {
		id = 5,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["COMMENT KEY"] = {
		id = 6,
		fgColor = rgb(0x8050A0),
		bgColor = rgb(defaultBg),
	},
	["COMMENT KEY ERROR"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(defaultBg),
	},
	["IDENTIFIER"] = {
		id = 8,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["VARIABLE"] = {
		id = 9,
		fgColor = rgb(0x008000),
		bgColor = rgb(defaultBg),
	},
	["ANONYMOUS"] = {
		id = 10,
		fgColor = rgb(0xA0A0C0),
		bgColor = rgb(defaultBg),
	},
	["NUMBER"] = {
		id = 11,
		fgColor = rgb(0x0000C0),
		bgColor = rgb(defaultBg),
	},
	["OPERATOR"] = {
		id = 12,
		fgColor = rgb(0xA00000),
		bgColor = rgb(defaultBg),
	},
	["CHARACTER"] = {
		id = 13,
		fgColor = rgb(0x800000),
		bgColor = rgb(defaultBg),
	},
	["CHARACTER TOO MANY"] = {
		id = 14,
		fgColor = rgb(0x800000),
		bgColor = rgb(defaultBg),
	},
	["CHARACTER ESCAPE ERROR"] = {
		id = 15,
		fgColor = rgb(0x800000),
		bgColor = rgb(defaultBg),
	},
	["STRING"] = {
		id = 16,
		fgColor = rgb(0x3898B2),
		bgColor = rgb(defaultBg),
	},
	["STRING ESCAPE"] = {
		id = 17,
		fgColor = rgb(0xFF8040),
		bgColor = rgb(defaultBg),
	},
	["STRING ESCAPE ERROR"] = {
		id = 18,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(defaultBg),
	},
	["STRING EOL OPEN"] = {
		id = 19,
		fgColor = rgb(defaultBg),
		bgColor = rgb(0xFF4040),
	},
	["STRING VERBATIM"] = {
		id = 20,
		fgColor = rgb(0x8064A2),
		bgColor = rgb(defaultBg),
	},
	["STRING VERBATIM SPECIAL"] = {
		id = 21,
		fgColor = rgb(0x800040),
		bgColor = rgb(defaultBg),
	},
	["STRING VERBATIM EOL"] = {
		id = 22,
		fgColor = rgb(defaultBg),
		bgColor = rgb(0xFF8000),
	},
}
return L
