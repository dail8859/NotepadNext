local L = {}

L.lexer = "postscript"

L.singleLineComment = "% "

L.extensions = {
	"ps",
}

L.keywords = {
	[0] = "$error = == FontDirectory StandardEncoding UserObjects abs add aload anchorsearch and arc arcn arcto array ashow astore atan awidthshow begin bind bitshift bytesavailable cachestatus ceiling charpath clear cleardictstack cleartomark clip clippath closefile closepath concat concatmatrix copy copypage cos count countdictstack countexecstack counttomark currentcmykcolor currentcolorspace currentdash currentdict currentfile currentflat currentfont currentgray currenthsbcolor currentlinecap currentlinejoin currentlinewidth currentmatrix currentmiterlimit currentpagedevice currentpoint currentrgbcolor currentscreen currenttransfer cvi cvlit cvn cvr cvrs cvs cvx def defaultmatrix definefont dict dictstack div dtransform dup echo end eoclip eofill eq erasepage errordict exch exec execstack executeonly executive exit exp false file fill findfont flattenpath floor flush flushfile for forall ge get getinterval grestore grestoreall gsave gt idetmatrix idiv idtransform if ifelse image imagemask index initclip initgraphics initmatrix inustroke invertmatrix itransform known kshow le length lineto ln load log loop lt makefont mark matrix maxlength mod moveto mul ne neg newpath noaccess nor not null nulldevice or pathbbox pathforall pop print prompt pstack put putinterval quit rand rcheck rcurveto read readhexstring readline readonly readstring rectstroke repeat resetfile restore reversepath rlineto rmoveto roll rotate round rrand run save scale scalefont search setblackgeneration setcachedevice setcachelimit setcharwidth setcolorscreen setcolortransfer setdash setflat setfont setgray sethsbcolor setlinecap setlinejoin setlinewidth setmatrix setmiterlimit setpagedevice setrgbcolor setscreen settransfer setvmthreshold show showpage sin sqrt srand stack start status statusdict stop stopped store string stringwidth stroke strokepath sub systemdict token transform translate true truncate type ueofill undefineresource userdict usertime version vmstatus wcheck where widthshow write writehexstring writestring xcheck xor",
	[1] = "GlobalFontDirectory ISOLatin1Encoding SharedFontDirectory UserObject arct colorimage cshow currentblackgeneration currentcacheparams currentcmykcolor currentcolor currentcolorrendering currentcolorscreen currentcolorspace currentcolortransfer currentdevparams currentglobal currentgstate currenthalftone currentobjectformat currentoverprint currentpacking currentpagedevice currentshared currentstrokeadjust currentsystemparams currentundercolorremoval currentuserparams defineresource defineuserobject deletefile execform execuserobject filenameforall fileposition filter findencoding findresource gcheck globaldict glyphshow gstate ineofill infill instroke inueofill inufill inustroke languagelevel makepattern packedarray printobject product realtime rectclip rectfill rectstroke renamefile resourceforall resourcestatus revision rootfont scheck selectfont serialnumber setbbox setblackgeneration setcachedevice2 setcacheparams setcmykcolor setcolor setcolorrendering setcolorscreen setcolorspace setcolortranfer setdevparams setfileposition setglobal setgstate sethalftone setobjectformat setoverprint setpacking setpagedevice setpattern setshared setstrokeadjust setsystemparams setucacheparams setundercolorremoval setuserparams setvmthreshold shareddict startjob uappend ucache ucachestatus ueofill ufill undef undefinefont undefineresource undefineuserobject upath ustroke ustrokepath vmreclaim writeobject xshow xyshow yshow",
	[2] = "cliprestore clipsave composefont currentsmoothness findcolorrendering setsmoothness shfill",
	[3] = ".begintransparencygroup .begintransparencymask .bytestring .charboxpath .currentaccuratecurves .currentblendmode .currentcurvejoin .currentdashadapt .currentdotlength .currentfilladjust2 .currentlimitclamp .currentopacityalpha .currentoverprintmode .currentrasterop .currentshapealpha .currentsourcetransparent .currenttextknockout .currenttexturetransparent .dashpath .dicttomark .discardtransparencygroup .discardtransparencymask .endtransparencygroup .endtransparencymask .execn .filename .fileposition .forceput .forceundef .forgetsave .getbitsrect .getdevice .inittransparencymask .knownget .locksafe .makeoperator .namestring .oserrno .oserrorstring .peekstring .rectappend .runandhide .setaccuratecurves .setblendmode .setcurvejoin .setdashadapt .setdebug .setdefaultmatrix .setdotlength .setfilladjust2 .setlimitclamp .setmaxlength .setopacityalpha .setoverprintmode .setrasterop .setsafe .setshapealpha .setsourcetransparent .settextknockout .settexturetransparent .stringbreak .stringmatch .tempfile .type1decrypt .type1encrypt .type1execchar .unread arccos arcsin copydevice copyscanlines currentdevice finddevice findlibfile findprotodevice flushpage getdeviceprops getenv makeimagedevice makewordimagedevice max min putdeviceprops setdevice",
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
	["DSC COMMENT"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["DSC VALUE"] = {
		id = 3,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["Name"] = {
		id = 5,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTRUCTION"] = {
		id = 6,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["LITERAL"] = {
		id = 7,
		fgColor = rgb(0x0080C0),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["IMMEVAL"] = {
		id = 8,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PAREN ARRAY"] = {
		id = 9,
		fgColor = rgb(0xFF00FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PAREN DICT"] = {
		id = 10,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PAREN PROC"] = {
		id = 11,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["TEXT"] = {
		id = 12,
		fgColor = rgb(0x808000),
		bgColor = rgb(0xFFFFFF),
	},
	["HEX STRING"] = {
		id = 13,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["BASE85 STRING"] = {
		id = 14,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["BAD STRING CHAR"] = {
		id = 15,
		fgColor = rgb(0x804000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
}
return L
