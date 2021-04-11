local L = {}

L.lexer = "asp"

L.extensions = {
	"asp",
}

L.keywords = {
	[0] = "addhandler addressof andalso alias and ansi as assembly attribute auto begin boolean byref byte byval call case catch cbool cbyte cchar cdate cdec cdbl char cint class clng cobj compare const continue cshort csng cstr ctype currency date decimal declare default delegate dim do double each else elseif end enum erase error event exit explicit false finally for friend function get gettype global gosub goto handles if implement implements imports in inherits integer interface is let lib like load long loop lset me mid mod module mustinherit mustoverride mybase myclass namespace new next not nothing notinheritable notoverridable object on option optional or orelse overloads overridable overrides paramarray preserve private property protected public raiseevent readonly redim rem removehandler rset resume return select set shadows shared short single static step stop string structure sub synclock then throw to true try type typeof unload unicode until variant wend when while with withevents writeonly xor",
}

L.styles = {
	["DEFAULT"] = {
		id = 81,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xC4F9FD),
	},
	["COMMENTLINE"] = {
		id = 82,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xC4F9FD),
	},
	["NUMBER"] = {
		id = 83,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xC4F9FD),
	},
	["WORD"] = {
		id = 84,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xC4F9FD),
		fontStyle = 1,
	},
	["STRING"] = {
		id = 85,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xC4F9FD),
	},
	["IDENTIFIER"] = {
		id = 86,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xC4F9FD),
	},
	["ASPSYMBOL"] = {
		id = 15,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFF00),
		fontStyle = 1,
	},
	["SCRIPTTYPE"] = {
		id = 16,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFC000),
	},
}
return L
