local L = {}

L.lexer = "inno"

L.extensions = {
	"iss",
}

L.keywords = {
	[0] = "code components custommessages dirs files icons ini installdelete langoptions languages messages registry run setup types tasks uninstalldelete uninstallrun _istool",
	[1] = "allowcancelduringinstall allownoicons allowrootdirectory allowuncpath alwaysrestart alwaysshowcomponentslist alwaysshowdironreadypage alwaysshowgrouponreadypage alwaysusepersonalgroup appcomments appcontact appcopyright appenddefaultdirname appenddefaultgroupname appid appmodifypath appmutex appname apppublisher apppublisherurl appreadmefile appsupporturl appupdatesurl appvername appversion architecturesallowed architecturesinstallin64bitmode backcolor backcolor2 backcolordirection backsolid changesassociations changesenvironment compression copyrightfontname copyrightfontsize createappdir createuninstallregkey defaultdirname defaultgroupname defaultuserinfoname defaultuserinfoorg defaultuserinfoserial dialogfontname dialogfontsize direxistswarning disabledirpage disablefinishedpage disableprogramgrouppage disablereadymemo disablereadypage disablestartupprompt diskclustersize diskslicesize diskspanning enablesdirdoesntexistwarning encryption extradiskspacerequired flatcomponentslist infoafterfile infobeforefile internalcompresslevel languagedetectionmethod languagecodepage languageid languagename licensefile lzmanumfastbytes mergeduplicatefiles minversion onlybelowversion outputbasefilename outputdir outputmanifestfile password privilegesrequired reservebytes restartifneededbyrun setupiconfile showcomponentsizes showlanguagedialog showtaskstreelines slicesperdisk solidcompression sourcedir timestamprounding timestampsinutc titlefontname titlefontsize touchdate touchtime uninstallable uninstalldisplayicon uninstalldisplayname uninstallfilesdir uninstalllogmode uninstallrestartcomputer updateuninstalllogappname usepreviousappdir usepreviousgroup useprevioussetuptype useprevioustasks useprevioususerinfo userinfopage usesetupldr versioninfocompany versioninfocopyright versioninfodescription versioninfotextversion versioninfoversion welcomefontname welcomefontsize windowshowcaption windowstartmaximized windowresizable windowvisible wizardimagebackcolor wizardimagefile wizardimagestretch wizardsmallimagefile",
	[2] = "afterinstall attribs beforeinstall check comment components copymode description destdir destname excludes extradiskspacerequired filename flags fontinstall groupdescription hotkey infoafterfile infobeforefile iconfilename iconindex key languages licensefile messagesfile minversion name onlybelowversion parameters permissions root runonceid section source statusmsg string subkey tasks type types valuedata valuename valuetype workingdir",
	[3] = "append define dim elif else emit endif endsub error expr file for if ifdef ifexist ifndef ifnexist include insert pragma sub undef",
	[4] = "begin break case const continue do downto else end except finally for function if of procedure repeat then to try until uses var while with",
	[5] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD"] = {
		id = 2,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PARAMETER"] = {
		id = 3,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["SECTION"] = {
		id = 4,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PREPROCESSOR"] = {
		id = 5,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PREPROCESSOR INLINE"] = {
		id = 6,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT PASCAL"] = {
		id = 7,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD PASCAL"] = {
		id = 8,
		fgColor = rgb(0x0080C0),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORD USER"] = {
		id = 9,
		fgColor = rgb(0x8080FF),
		bgColor = rgb(0xFFFFCC),
		fontStyle = 1,
	},
	["STRING DOUBLE"] = {
		id = 10,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING SINGLE"] = {
		id = 11,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 12,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
