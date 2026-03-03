local L = {}

L.lexer = "autoit"

L.singleLineComment = "; "

L.extensions = {
	"au3",
}

L.keywords = {
	[0] = "and byref case const continuecase continueloop default dim do else elseif endfunc endif endselect endswitch enum exit exitloop false for func global if in local next not null or redim return select static step switch then to true until wend while",
	[1] = "abs acos adlibdisable adlibenable asc asin atan autoitsetoption autoitwingettitle autoitwinsettitle bitand bitnot bitor bitshift bitxor blockinput break call cdtray chr clipget clipput controlclick controlcommand controldisable controlenable controlfocus controlgetfocus controlgetpos controlgettext controlhide controlmove controlsend controlsettext controlshow cos dec dircopy dircreate dirmove dirremove drivegetdrive drivegetfilesystem drivegetlabel drivegetserial drivegettype drivesetlabel drivespacefree drivespacetotal drivestatus envget envset envupdate eval exp filechangedir fileclose filecopy filecreateshortcut filedelete fileexists filefindfirstfile filefindnextfile filegetattrib filegetlongname filegetshortname filegetsize filegettime filegetversion fileinstall filemove fileopen fileopendialog fileread filereadline filerecycle filerecycleempty filesavedialog fileselectfolder filesetattrib filesettime filewrite filewriteline guicreate guicreateex guidefaultfont guidelete guigetcontrolstate guihide guimsg guiread guirecvmsg guisendmsg guisetcontrol guisetcontroldata guisetcontrolex guisetcontrolfont guisetcontrolnotify guisetcoord guisetcursor guishow guiwaitclose guiwrite hex hotkeyset inidelete iniread iniwrite inputbox int isadmin isarray isdeclared isfloat isint isnumber isstring log memgetstats mod mouseclick mouseclickdrag mousedown mousegetcursor mousegetpos mousemove mouseup mousewheel msgbox number pixelchecksum pixelgetcolor pixelsearch processclose processexists processsetpriority processwait processwaitclose progressoff progresson progressset random regdelete regenumkey regenumval regread regwrite round run runasset runwait send seterror shutdown sin sleep soundplay soundsetwavevolume splashimageon splashoff splashtexton sqrt statusbargettext string stringaddcr stringformat stringinstr stringisalnum stringisalpha stringisascii stringisdigit stringisfloat stringisint stringislower stringisspace stringisupper stringisxdigit stringleft stringlen stringlower stringmid stringreplace stringright stringsplit stringstripcr stringstripws stringtrimleft stringtrimright stringupper tan timerstart timerstop tooltip traytip ubound urldownloadtofile winactivate winactive winclose winexists wingetcaretpos wingetclasslist wingetclientsize wingethandle wingetpos wingetstate wingettext wingettitle winkill winmenuselectitem winminimizeall winminimizeallundo winmove winsetontop winsetstate winsettitle winwait winwaitactive winwaitclose winwaitnotactive",
	[2] = "@appdatacommondir @appdatadir @autoitexe @autoitpid @autoitversion @autoitx64 @com_eventobj @commonfilesdir @compiled @computername @comspec @cpuarch @cr @crlf @desktopcommondir @desktopdepth @desktopdir @desktopheight @desktoprefresh @desktopwidth @documentscommondir @error @exitcode @exitmethod @extended @favoritescommondir @favoritesdir @gui_ctrlhandle @gui_ctrlid @gui_dragfile @gui_dragid @gui_dropid @gui_winhandle @homedrive @homepath @homeshare @hotkeypressed @hour @ipaddress1 @ipaddress2 @ipaddress3 @ipaddress4 @kblayout @lf @localappdatadir @logondnsdomain @logondomain @logonserver @mday @min @mon @msec @muilang @mydocumentsdir @numparams @osarch @osbuild @oslang @osservicepack @ostype @osversion @programfilesdir @programscommondir @programsdir @scriptdir @scriptfullpath @scriptlinenumber @scriptname @sec @startmenucommondir @startmenudir @startupcommondir @startupdir @sw_disable @sw_enable @sw_hide @sw_lock @sw_maximize @sw_minimize @sw_restore @sw_show @sw_showdefault @sw_showmaximized @sw_showminimized @sw_showminnoactive @sw_showna @sw_shownoactivate @sw_shownormal @sw_unlock @systemdir @tab @tempdir @tray_id @trayiconflashing @trayiconvisible @username @userprofiledir @wday @windowsdir @workingdir @yday @year",
	[3] = "{!} {#} {^} {{} {}} {+} {alt} {altdown} {altup} {appskey} {asc} {backspace} {browser_back} {browser_favorites} {browser_forward} {browser_home} {browser_refresh} {browser_search} {browser_stop} {bs} {capslock} {ctrlbreak} {ctrldown} {ctrlup} {del} {delete} {down} {end} {enter} {esc} {escape} {f1} {f10} {f11} {f12} {f2} {f3} {f4} {f5} {f6} {f7} {f8} {f9} {home} {ins} {insert} {lalt} {launch_app1} {launch_app2} {launch_mail} {launch_media} {lctrl} {left} {lshift} {lwin} {lwindown} {media_next} {media_play_pause} {media_prev} {media_stop} {numlock} {numpad0} {numpad1} {numpad2} {numpad3} {numpad4} {numpad5} {numpad6} {numpad7} {numpad8} {numpad9} {numpadadd} {numpaddiv} {numpaddot} {numpadenter} {numpadmult} {numpadsub} {pause} {pgdn} {pgup} {printscreen} {ralt} {rctrl} {right} {rshift} {rwin} {rwindown} {scrolllock} {shiftdown} {shiftup} {sleep} {space} {tab} {up} {volume_down} {volume_mute} {volume_up}",
	[4] = "#include #include-once #pragma",
	[5] = "#region #endregion",
	[6] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["FUNCTION"] = {
		id = 4,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["MACRO"] = {
		id = 6,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 8,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["VARIABLE"] = {
		id = 9,
		fgColor = rgb(0xFF0080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 2,
	},
	["SENT"] = {
		id = 10,
		fgColor = rgb(0x8080C0),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROCESSOR"] = {
		id = 11,
		fgColor = rgb(0x804000),
		bgColor = rgb(0xFFFFFF),
	},
	["SPECIAL"] = {
		id = 12,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["EXPAND"] = {
		id = 13,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMOBJ"] = {
		id = 14,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
