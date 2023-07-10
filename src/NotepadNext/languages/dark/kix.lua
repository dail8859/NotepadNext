local L = {}

L.lexer = "kix"

L.extensions = {
	"kix",
}

L.keywords = {
	[0] = "? and beep big break call cd cls color cookie1 copy debug del dim display do until exit flushkb for each next function endfunction get gets global go gosub goto if else endif md or password play quit rd redim return run select case endselect set setl setm settime shell sleep small use while loop",
	[1] = "abs addkey addprinterconnection addprogramgroup addprogramitem asc ascan at backupeventlog box cdbl chr cint cleareventlog close comparefiletimes createobject cstr dectohex delkey delprinterconnection delprogramgroup delprogramitem deltree delvalue dir enumgroup enumipinfo enumkey enumlocalgroup enumvalue execute exist existkey expandenvironmentvars fix formatnumber freefilehandle getdiskspace getfileattr getfilesize getfiletime getfileversion getobject iif ingroup instr instrrev int isdeclared join kbhit keyexist lcase left len loadhive loadkey logevent logoff ltrim memorysize messagebox open readline readprofilestring readtype readvalue redirectoutput right rnd round rtrim savekey sendkeys sendmessage setascii setconsole setdefaultprinter setfileattr setfocus setoption setsystemstate settitle setwallpaper showprogramgroup shutdown sidtoname split srnd substr trim ubound ucase unloadhive val vartype vartypename writeline writeprofilestring writevalue",
	[2] = "address build color comment cpu crlf csd curdir date day domain dos error fullname homedir homedrive homeshr hostname inwin ipaddress0 ipaddress1 ipaddress2 ipaddress3 kix lanroot ldomain ldrive lm logonmode longhomedir lserver maxpwage mdayno mhz monthno month msecs pid primarygroup priv productsuite producttype pwage ras result rserver scriptdir scriptexe scriptname serror sid site startdir syslang ticks time userid userlang wdayno wksta wuserid ydayno year",
}

L.styles = {
	["DEFAULT"] = {
		id = 31,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["STRING"] = {
		id = 2,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["STRING2"] = {
		id = 3,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["VAR"] = {
		id = 5,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["MACRO"] = {
		id = 6,
		fgColor = rgb(cppTypeColor),
		bgColor = darkBg,
	},
	["INSTRUCTION WORD"] = {
		id = 7,
		fgColor = rgb(cppInstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["FUNCTION"] = {
		id = 8,
		fgColor = rgb(cppTypeColor),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 9,
		fgColor = rgb(cppOperatorColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
}
return L
