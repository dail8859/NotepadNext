local L = {}

L.lexer = "freebasic"

L.singleLineComment = "' "

L.extensions = {
	"bas",
	"bi",
}

L.keywords = {
	[0] = "append as asc asin asm atan2 atn beep bin binary bit bitreset bitset bload bsave byref byte byval call callocate case cbyte cdbl cdecl chain chdir chr cint circle clear clng clngint close cls color command common cons const continue cos cshort csign csng csrlin cubyte cuint culngint cunsg curdir cushort custom cvd cvi cvl cvlongint cvs cvshort data date deallocate declare defbyte defdbl defined defint deflng deflngint defshort defsng defstr defubyte defuint defulngint defushort dim dir do double draw dylibload dylibsymbol else elseif end enum environ environ$ eof eqv erase err error exec exepath exit exp export extern field fix flip for fre freefile function get getjoystick getkey getmouse gosub goto hex hibyte hiword if iif imagecreate imagedestroy imp inkey inp input instr int integer is kill lbound lcase left len let lib line lobyte loc local locate lock lof log long longint loop loword lset ltrim mid mkd mkdir mki mkl mklongint mks mkshort mod multikey mutexcreate mutexdestroy mutexlock mutexunlock name next not oct on once open option or out output overload paint palette pascal pcopy peek peeki peeks pipe pmap point pointer poke pokei pokes pos preserve preset print private procptr pset ptr public put random randomize read reallocate redim rem reset restore resume resume next return rgb rgba right rmdir rnd rset rtrim run sadd screen screencopy screeninfo screenlock screenptr screenres screenset screensync screenunlock seek statement seek function selectcase setdate setenviron setmouse settime sgn shared shell shl short shr sin single sizeof sleep space spc sqr static stdcall step stop str string string strptr sub swap system tab tan then threadcreate threadwait time time timer to trans trim type ubound ubyte ucase uinteger ulongint union unlock unsigned until ushort using va_arg va_first va_next val val64 valint varptr view viewprint wait wend while width window windowtitle with write xor zstring",
	[1] = "#define #dynamic #else #endif #error #if #ifdef #ifndef #inclib #include #print #static #undef",
	[2] = "None",
	[3] = "None",
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
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0x007F7F),
		bgColor = darkBg,
	},
	["KEYWORD1"] = {
		id = 3,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["STRING"] = {
		id = 4,
		fgColor = rgb(0x7F007F),
		bgColor = darkBg,
	},
	["PREPROCESSOR"] = {
		id = 5,
		fgColor = rgb(0x7F7F00),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 6,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["IDENTIFIER"] = {
		id = 7,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["KEYWORD2"] = {
		id = 10,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
	},
	["KEYWORD3"] = {
		id = 11,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
	},
	["KEYWORD4"] = {
		id = 12,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
	},
	["LABEL"] = {
		id = 15,
		fgColor = rgb(0x7F7F00),
		bgColor = darkBg,
	},
	["ERROR"] = {
		id = 16,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["HEXNUMBER"] = {
		id = 17,
		fgColor = rgb(0x007F7F),
		bgColor = darkBg,
	},
	["BINNUMBER"] = {
		id = 18,
		fgColor = rgb(0x007F7F),
		bgColor = darkBg,
	},
}
return L
