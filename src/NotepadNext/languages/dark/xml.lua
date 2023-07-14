local L = {}

L.lexer = "xml"

L.first_line = {
	"^<%?xml"
}

L.extensions = {
	"xml",
	"xaml",
	"xsl",
	"xslt",
	"xsd",
	"xul",
	"kml",
	"svg",
	"mxml",
	"xsml",
	"wsdl",
	"xlf",
	"xliff",
	"xbl",
	"sxbl",
	"sitemap",
	"gml",
	"gpx",
	"plist",
	"vcproj",
	"vcxproj",
	"csproj",
	"csxproj",
	"vbproj",
	"dbproj",
}

L.keywords = {
}

L.styles = {
	["XMLSTART"] = {
		id = 12,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFF00),
	},
	["XMLEND"] = {
		id = 13,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFF00),
	},
	["DEFAULT"] = {
		id = 0,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 9,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 5,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
	},
	["DOUBLESTRING"] = {
		id = 6,
		fgColor = rgb(TypeColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["SINGLESTRING"] = {
		id = 7,
		fgColor = rgb(TypeColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["TAG"] = {
		id = 1,
		fgColor = rgb(InstructionColor),
		bgColor = darkBg,
	},
	["TAGEND"] = {
		id = 11,
		fgColor = rgb(InstructionColor),
		bgColor = darkBg,
	},
	["TAGUNKNOWN"] = {
		id = 2,
		fgColor = rgb(InstructionColor),
		bgColor = darkBg,
	},
	["ATTRIBUTE"] = {
		id = 3,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
	},
	["ATTRIBUTEUNKNOWN"] = {
		id = 4,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
	},
	["SGMLDEFAULT"] = {
		id = 21,
		fgColor = darkFg,
		bgColor = rgb(0xA6CAF0),
	},
	["CDATA"] = {
		id = 17,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["ENTITY"] = {
		id = 10,
		fgColor = darkFg,
		bgColor = rgb(0xFEFDE0),
		fontStyle = 2,
	},
}
return L
