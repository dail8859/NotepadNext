local L = {}

L.lexer = "escript"

L.singleLineComment = "// "

L.extensions = {
	"src",
	"em",
}

L.keywords = {
	[0] = "basic basicio boats cfgfile file http npc os uo util accessible addmenuitem appendconfigfileelem applyconstraint applydamage applyrawdamage assignrecttoweatherregion append baseskilltorawskill boatfromitem broadcast ban cdbl cint cstr checklineofsight checklosat checkskill consumemana consumereagents consumesubstance createaccount createitematlocation createiteminbackpack createitemininventory createitemincontainer createmenu createmultiatlocation createnpcfromtemplate createrootiteminstoragearea createstoragearea clear_script_profile_counters close damage destroyitem destroymulti destroyrootiteminstoragearea detach disableevents disconnectclient distance disable enableevents enumerateitemsincontainer enumerateonlinecharacters equipfromtemplate equipitem eraseglobalproperty eraseobjproperty enable enabled erase events_waiting exists findconfigelem findobjtypeincontainer findrootiteminstoragearea findstoragearea fclose find fopen fread fseek ftell fwrite gamestat getamount getcommandhelp getconfigint getconfigintkeys getconfigmaxintkey getconfigreal getconfigstring getconfigstringkeys getconfigstringarray getelemproperty getequipmentbylayer getglobalproperty getharvestdifficulty getmapinfo getmenuobjtypes getobjproperty getobjtype getobjtypebyname getproperty getrawskill getregionstring getskill getspelldifficulty getstandingheight getworldheight grantprivilege harvestresource healdamage hex islegalmove insert keys listequippeditems listghostsnearlocation listhostiles listitemsatlocation listitemsnearlocation listitemsnearlocationoftype listmobilesinlineofsight listmobilesnearlocation listmobilesnearlocationex listobjectsinbox loadtusscpfile left len log_profile lower makeboundingbox move moveboat moveboatrelative movecharactertolocation moveitemtocontainer moveitemtolocation move_offline_mobiles openpaperdoll open pack performaction playlightningbolteffect playmovingeffect playmovingeffectxyz playobjectcenteredeffect playsoundeffect playsoundeffectprivate playstationaryeffect printtextabove printtextaboveprivate packages polcore position print queryparam randomdiceroll randomint rawskilltobaseskill readconfigfile readgameclock releaseitem registerforspeechevents registeritemwithboat requestinput reserveitem restartscript resurrect revokeprivilege runawayfrom runawayfromlocation runtoward runtowardlocation reverse run_script_to_completion saveworldstate selectmenuitem2 self sendbuywindow senddialoggump sendevent sendopenspecialcontainer sendpacket sendsellwindow sendskillwindow sendstringastipwindow sendsysmessage sendtextentrygump setanchor setglobalproperty setname setobjproperty setopponent setproperty setrawskill setregionlightlevel setregionweatherlevel setscriptcontroller setwarmode shutdown speakpowerwords splitwords startspelleffect subtractamount systemfindboatbyserial systemfindobjectbyserial say set_critical set_debug set_priority set_priority_divide set_script_option setcmdlevel setdex setint setlightlevel setmaster setname setpassword setstr shrink size sleep sleepms sort spendgold squelch start_script syslog system_rpm target targetcoordinates targetmultiplacement turnawayfrom turnawayfromlocation turnboat turntoward turntowardlocation toggle unloadconfigfile unpack unban unload_scripts upper walkawayfrom walkawayfromlocation walktoward walktowardlocation wander writehtml writehtmlraw wait_for_event movechar_forcelocation moveitem_forcelocation moveitem_normal scriptopt_debug scriptopt_no_interrupt scriptopt_no_runaway te_cancel_disable te_cancel_enable te_style_disable te_style_normal te_style_numerical tgtopt_check_los tgtopt_harmful tgtopt_helpful tgtopt_neutral tgtopt_nocheck_los setprop getprop",
	[1] = "array const dictionary global local var and default in next not or return to include use enum",
	[2] = "while for endfor function program endprogram endfunction foreach case else elseif if endcase endenum endforeach endif endwhile",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = darkBg,
	},
	["LINE COMMENT"] = {
		id = 2,
		fgColor = rgb(0x007F00),
		bgColor = darkBg,
	},
	["DOC COMMENT"] = {
		id = 3,
		fgColor = rgb(0x3F703F),
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0x007F7F),
		bgColor = darkBg,
	},
	["KEYWORD"] = {
		id = 5,
		fgColor = rgb(0xFF4000),
		bgColor = darkBg,
	},
	["DOUBLE QUOTED STRING"] = {
		id = 6,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["OPERATORS"] = {
		id = 7,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
	},
	["IDENTIFIERS"] = {
		id = 8,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["BRACES"] = {
		id = 9,
		fgColor = rgb(cppInstructionColor),
		bgColor = darkBg,
	},
	["KEYWORDS2"] = {
		id = 10,
		fgColor = rgb(0xB00040),
		bgColor = darkBg,
	},
	["KEYWORDS3"] = {
		id = 11,
		fgColor = rgb(0xB00040),
		bgColor = darkBg,
	},
}
return L
