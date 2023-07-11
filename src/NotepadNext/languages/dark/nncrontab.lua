local L = {}

L.lexer = "nncrontab"

L.singleLineComment = "# "

L.extensions = {
	"tab",
	"spf",
}

L.keywords = {
	[0] = "AGAIN ALLOT AND Action BEGIN CASE COMPARE CONSTANT CREATE DO Days ELSE ENDCASE ENDOF EVAL-SUBST EVALUATE FALSE Hours I IF LEAVE LOOP Minutes Months NOT OF OFF ON OR PAD REPEAT Rule SET THEN TRUE Time UNTIL VALUE VARIABLE WHILE WeekDays Years",
	[1] = "</JScript> </SCRIPT> </VBScript> <JScript> <VBScript> ACCESS-DATE BEEP CALL_DIAL CALL_HANGUP CHAR CLIPBOARD CONSOLE CREATION-DATE CUR-DATE DATE- DATE-INTERVAL DELAY DIAL DIR-CREATE DIR-DELETE DIR-EMPTY DIR-SIZE Day@ Domain ERR-MSG EXIST FILE-ACCESS-DATE FILE-APPEND FILE-COPY FILE-CREATE FILE-CREATION-DATE FILE-CROP FILE-DATE FILE-DELETE FILE-EMPTY FILE-EXIST FILE-MOVE FILE-RENAME FILE-SIZE FILE-WRITE FILE-WRITE-DATE FOR-CHILD-WINDOWS FOR-FILES FOR-WINDOWS FREE-SPACE GET-CUR-TIME GET-REG GET-VER GetTickCount HANGUP HIDE-ICON HINT HINT-OFF HINT-POS HINT-SIZE HINTW HOST-EXIST Hour@ IDLE INTERVAL IS-ARCHIVE IS-DIR IS-HIDDEN IS-READONLY IS-SYSTEM KILL LAUNCH LOG LOGGEDON LOGOFF LogonBatch LogonInteractive LogonNetwork MOUSE-LBCLK MOUSE-LBDCLK MOUSE-LBDN MOUSE-LBUP MOUSE-MOVE MOUSE-MOVER MOUSE-MOVEW MOUSE-RBCLK MOUSE-RBDCLK MOUSE-RBDN MOUSE-RBUP MSG Min@ Mon@ MonitorResponseTime NHOST-EXIST No ONLINE PAUSE PLAY-SOUND PLAY-SOUNDW POP3-CHECK POWEROFF PROC-EXIST PROC-TIME PURGE-OLD PURGE-OLDA PURGE-OLDW Password QSTART-APP QSTART-APPW QUERY QUIT RASDomain RASError RASPassword RASPhone RASSecPassword RASUser RE-ALL RE-MATCH REBOOT REG-DELETE-KEY REG-DELETE-VALUE REG-DWORD REG-SZ REMINDER RUN SEND-KEYS SEND-KEYS-DELAY SHOW-ICON SHUTDOWN START-APP START-APPW START-QUIT START-TIME Sec@ SecPassword THINT THINTW TimeSec@ TMSG TQUERY User WDay@ WIN-ACTIVATE WIN-ACTIVE WIN-CLICK WIN-CLOSE WIN-EXIST WIN-HIDE WIN-HWND WIN-MAXIMIZE WIN-MINIMIZE WIN-MOVE WIN-MOVER WIN-RESTORE WIN-SEND-KEYS WIN-SHOW WIN-TERMINATE WIN-TOPMOST WIN-VER WIN-WAIT WINAPI WRITE-DATE WatchClipboard WatchConnect WatchDir WatchDisconnect WatchDriveInsert WatchDriveRemove WatchFile WatchProc WatchProcStop WatchWinActivate WatchWinCreate WatchWinDestroy WatchWindow WinNT Year@ Yes",
	[2] = "ALL AboveNormalPriority AsService BelowNormalPriority FILESONLY HighPriority IdlePriority LoadProfile NoActive NoDel NoLog NoRunAs NormalPriority OnceADay OnceAHour OnceAMonth OnceAWeek RECURSIVE RealtimePriority RunOnce SWHide ShowMaximized ShowMinimized ShowNoActivate ShowNormal StartIn StartPos StartSize TODEPTH WATCH-CHANGE-ATTRIBUTES WATCH-CHANGE-DIR-NAME WATCH-CHANGE-FILE-NAME WATCH-CHANGE-LAST-WRITE WATCH-CHANGE-SECURITY WATCH-CHANGE-SIZE WaitFor WatchSubtree WithoutProfile",
}

L.styles = {
	["WHITESPACE"] = {
		id = 0,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = darkBg,
	},
	["TASK START/END"] = {
		id = 2,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0x606060),
	},
	["SECTION KEYWORDS"] = {
		id = 3,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["KEYWORDS"] = {
		id = 4,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
	["MODIFICATORS"] = {
		id = 5,
		fgColor = rgb(0x00007F),
		bgColor = darkBg,
		fontStyle = 2,
	},
	["ASTERISK"] = {
		id = 6,
		fgColor = darkFg,
		bgColor = rgb(0xFFE0FF),
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 7,
		fgColor = rgb(0x007F7F),
		bgColor = darkBg,
	},
	["DOUBLE QUOTED STRING"] = {
		id = 8,
		fgColor = rgb(0x7F007F),
		bgColor = darkBg,
	},
	["ENVIRONMENT VARIABLE"] = {
		id = 9,
		fgColor = darkFg,
		bgColor = rgb(0xFFFFE0),
	},
	["IDENTIFIER"] = {
		id = 10,
		fgColor = darkFg,
		bgColor = rgb(0xFF0000),
	},
}
return L