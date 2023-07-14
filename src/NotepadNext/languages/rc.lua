local L = {}

L.lexer = "rc"

L.singleLineComment = "// "

L.extensions = {
	"rc",
}

L.keywords = {
	[0] = "ACCELERATORS ALT AUTO3STATE AUTOCHECKBOX AUTORADIOBUTTON BEGIN BITMAP BLOCK BUTTON CAPTION CHARACTERISTICS CHECKBOX CLASS COMBOBOX CONTROL CTEXT CURSOR DEFPUSHBUTTON DIALOG DIALOGEX DISCARDABLE EDITTEXT END EXSTYLE FONT GROUPBOX ICON LANGUAGE LISTBOX LTEXT MENU MENUEX MENUITEM MESSAGETABLE POPUP PUSHBUTTON RADIOBUTTON RCDATA RTEXT SCROLLBAR SEPARATOR SHIFT STATE3 STRINGTABLE STYLE TEXTINCLUDE VALUE VERSION VERSIONINFO VIRTKEY",
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
