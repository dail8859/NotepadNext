local L = {}

L.lexer = "oscript"

L.singleLineComment = "// "

L.extensions = {
	"osx",
}

L.keywords = {
	[0] = "break breakif by case continue continueif default downto else elseif end for goto if in repeat switch to until while function return void dll inbyref inout linked nodebug super this xcmd xfcn",
	[1] = "false true undefined booleantype bytestype datatype dynamictype errortype externtype integertype listtype longtype objecttype objreftype pointtype realtype scripttype stringtype undefinedtype voidtype",
	[2] = "and eq ge gt le lt ne not or xor",
	[3] = "assoc boolean bytes date dynamic error extern file integer list long object point real recarray record script string cachetree capiconnect capierr capilog capilogin compiler dapisession dapinode dapiversion dapistream filecopy fileprefs frame javaobject mailmessage pop3session smtpsession ssloptions patfind patchange regex socket sqlconnection sqlcursor uapisession uapiuser wapisession wapimap wapimaptask wapiwork wapisubwork domattr domcdatasection domcharacterdata domcomment domdocument domdocumentfragment domdocumenttype domelement domentity domentityreference domimplementation domnamednodemap domnode domnodelist domnotation domparser domprocessinginstruction domtext saxparser xslprocessor",
	[4] = "datatypename echo echodebug echoerror echoinfo echostamp echowarn getfeatures isdefined iserror isfeature isinvokable isnoterror isnotset isobject isset isundefined length nparameters parameters pointh pointv type",
	[5] = "cachetree capiconnect capierr capilog capilogin compiler dapisession dapinode dapiversion dapistream filecopy fileprefs frame javaobject mailmessage pop3session smtpsession ssloptions patfind patchange regex socket sqlconnection sqlcursor uapisession uapiuser wapisession wapimap wapimaptask wapiwork wapisubwork domattr domcdatasection domcharacterdata domcomment domdocument domdocumentfragment domdocumenttype domelement domentity domentityreference domimplementation domnamednodemap domnode domnodelist domnotation domparser domprocessinginstruction domtext saxparser xslprocessor",
}

L.styles = {
	["DEFAULT TEXT STYLE"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["SINGLE-LINE COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
	},
	["MULTI-LINE COMMENT"] = {
		id = 2,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
	},
	["#IFDEF DOC AND #ENDIF"] = {
		id = 3,
		fgColor = rgb(0x3F703F),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROCESSOR DIRECTIVE"] = {
		id = 4,
		fgColor = rgb(0x7F7F00),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 5,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING SINGLE QUOTES"] = {
		id = 6,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING DOUBLE QUOTES"] = {
		id = 7,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["CONSTANT LITERAL"] = {
		id = 8,
		fgColor = rgb(0xFF7F3F),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 9,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["SERVER-GLOBAL VARIABLE (PREFIXED BY $)"] = {
		id = 10,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["LANGUAGE NATIVE KEYWORD OR RESERVED WORD"] = {
		id = 11,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["OPERATOR; EITHER SYMBOLIC OR LITERAL"] = {
		id = 12,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["LABEL TO JUMP TO WITH THE GOTO STATEMENT"] = {
		id = 13,
		fgColor = rgb(0xFF00FF),
		bgColor = rgb(0xFFFFFF),
	},
	["TYPE"] = {
		id = 14,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["FUNCTION"] = {
		id = 15,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["STATIC BUILT-IN OBJECT"] = {
		id = 16,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["OBJECT PROPERTY"] = {
		id = 17,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["OBJECT METHOD"] = {
		id = 18,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
