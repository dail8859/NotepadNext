local L = {}

L.lexer = "spice"

L.extensions = {
	"scp",
	"out",
}

L.keywords = {
	[0] = "ac alias alter alterparam append askvalues assertvalid autoscale break compose copy copytodoc dc delete destroy destroyvec diff display disto dowhile echo else end errorstop fftinit filter foreach fourier freqtotime function functionundef goto homecursors if isdisplayed label let linearize listing load loadaccumulator makelabel movelabel makesmithplot movecursorleft movecursorright msgbox nameplot newplot nextparam noise nopoints op plot plotf plotref poly print printcursors printevent printname printplot printstatus printtext printtol printunits printval printvector pwl pz quit removesmithplot rename repeat resume rotate runs rusage save sendplot sendscript sens set setcursor setdoc setlabel setlabeltype setmargins setnthtrigger setunits setvec setparam setplot setquery setscaletype settracecolor settracestyle setsource settrigger setvec setxlimits setylimits show showmod sort status step stop switch tf timetofreq timetowave tran unalias unlet unset unalterparam update version view wavefilter wavetotime where while write",
	[1] = "abs askvalue atan average ceil cos db differentiate differentiatex exp finalvalue floor getcursorx getcursory getcursory0 getcursory1 getparam im ln initialvalue integrate integratex interpolate isdef isdisplayed j log length mag, max maxscale mean meanpts min minscale nextplot nextvector norm operatingpoint ph phase phaseextend pk_pk pos pulse re rms rmspts rnd sameplot sin sqrt stddev stddevpts tan tfall tolerance trise unitvec vector",
	[2] = "param nodeset include options dcconv subckt ends model",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIERS"] = {
		id = 1,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD"] = {
		id = 2,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD2"] = {
		id = 3,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORD3"] = {
		id = 4,
		fgColor = rgb(0x0040E0),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 5,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATORS (DELIMITERS)"] = {
		id = 6,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["VALUE"] = {
		id = 7,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 8,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
