local L = {}

L.lexer = "javascript"

L.keywords = {
	[0] = "abstract async await boolean break byte case catch char class const continue debugger default delete do double else enum export extends final finally float for function goto if implements import in instanceof int interface let long native new null of package private protected public return short static super switch synchronized this throw throws transient try typeof var void volatile while with true false prototype yield",
}

L.styles = {
	["DEFAULT"] = {
		id = 41,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xF2F4FF),
	},
	["NUMBER"] = {
		id = 45,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xF2F4FF),
	},
	["WORD"] = {
		id = 46,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xF2F4FF),
	},
	["KEYWORD"] = {
		id = 47,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xF2F4FF),
		fontStyle = 3,
	},
	["DOUBLESTRING"] = {
		id = 48,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xF2F4FF),
	},
	["SINGLESTRING"] = {
		id = 49,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xF2F4FF),
	},
	["SYMBOLS"] = {
		id = 50,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xF2F4FF),
		fontStyle = 1,
	},
	["REGEX"] = {
		id = 52,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xF2F4FF),
	},
	["COMMENT"] = {
		id = 42,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xF2F4FF),
	},
	["COMMENTLINE"] = {
		id = 43,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xF2F4FF),
	},
	["COMMENTDOC"] = {
		id = 44,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xF2F4FF),
	},
}
return L
