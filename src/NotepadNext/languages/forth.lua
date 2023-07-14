local L = {}

L.lexer = "forth"

L.singleLineComment = "\\ "

L.extensions = {
	"forth",
}

L.keywords = {
	[0] = "AGAIN BEGIN CASE DO ELSE ENDCASE ENDOF IF LOOP OF REPEAT THEN UNTIL  WHILE [IF] [ELSE] [THEN] ?DO",
	[1] = "DUP DROP ROT SWAP OVER @ ! 2@ 2! 2DUP 2DROP 2SWAP 2OVER NIP R@ >R R> 2R@ 2>R 2R> 0= 0< SP@ SP! W@ W! C@ C! < > = <> 0<>SPACE SPACES KEY? KEY THROW CATCH ABORT */ 2* /MOD CELL+ CELLS CHAR+ CHARS MOVE ERASE DABS TITLE HEX DECIMAL HOLD <# # #S #> SIGN D. . U. DUMP (.\") >NUMBER ' IMMEDIATE EXIT RECURSE UNLOOP LEAVE HERE ALLOT , C, W, COMPILE, BRANCH, RET, LIT, DLIT, ?BRANCH, \", >MARK >RESOLVE1 <MARK >RESOLVE ALIGN ALIGNED USER-ALLOT USER-HERE HEADER DOES> SMUDGE HIDE :NONAME LAST-WORD ?ERROR ERROR2 FIND1 SFIND SET-CURRENT GET-CURRENT DEFINITIONS GET-ORDER FORTH ONLY SET-ORDER ALSO PREVIOUS VOC-NAME. ORDER LATEST LITERAL 2LITERAL SLITERAL CLITERAL ?LITERAL1 ?SLITERAL1 HEX-LITERAL HEX-SLITERAL ?LITERAL2 ?SLITERAL2 SOURCE EndOfChunk CharAddr PeekChar IsDelimiter GetChar OnDelimiter SkipDelimiters OnNotDelimiter SkipWord SkipUpTo ParseWord NextWord PARSE SKIP CONSOLE-HANDLES REFILL DEPTH ?STACK ?COMP WORD INTERPRET BYE QUIT MAIN1 EVALUATE INCLUDE-FILE INCLUDED >BODY +WORD WORDLIST CLASS! CLASS@ PAR! PAR@ ID. ?IMMEDIATE ?VOC IMMEDIATE VOC WordByAddrWl WordByAddr NLIST WORDS SAVE OPTIONS /notransl ANSI>OEM ACCEPT EMIT CR TYPE EKEY? EKEY EKEY>CHAR EXTERNTASK ERASE-IMPORTS ModuleName ModuleDirName ENVIRONMENT? DROP-EXC-HANDLER SET-EXC-HANDLER HALT ERR CLOSE-FILE CREATE-FILE CREATE-FILE-SHARED OPEN-FILE-SHARED DELETE-FILE FILE-POSITION FILE-SIZE OPEN-FILE READ-FILE REPOSITION-FILE DOS-LINES UNIX-LINES READ-LINE WRITE-FILE RESIZE-FILE WRITE-LINE ALLOCATE FREE RESIZE START SUSPEND RESUME STOP PAUSE MIN MAX TRUE FALSE ASCIIZ> R/O W/O ;CLASS ENDWITH OR AND /STRING SEARCH COMPARE EXPORT ;MODULE SPACE",
	[2] = "VARIABLE CREATE : VALUE CONSTANT VM: M: var dvar chars OBJ CONSTR: DESTR: CLASS: OBJECT: POINTER USER USER-CREATE USER-VALUE VECT WNDPROC: VOCABULARY -- TASK: CEZ: MODULE:",
	[3] = "CHAR [CHAR] POSTPONE WITH ['] TO [COMPILE] CHAR ASCII \'",
	[4] = "REQUIRE WINAPI",
	[5] = "S\" ABORT\" Z\" \" .\" C\"",
}

L.styles = {
	["WHITESPACE"] = {
		id = 0,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x009933),
		bgColor = defaultBg,
	},
	["ML COMMENT"] = {
		id = 2,
		fgColor = rgb(0x007F00),
		bgColor = defaultBg,
	},
	["IDENTIFIER"] = {
		id = 3,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["CONTROL"] = {
		id = 4,
		fgColor = rgb(0x00007F),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["KEYWORDS"] = {
		id = 5,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["DEFWORDS"] = {
		id = 6,
		fgColor = rgb(0xFF0000),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["PREWORD1"] = {
		id = 7,
		fgColor = rgb(0xCC3300),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["PREWORD2"] = {
		id = 8,
		fgColor = rgb(0x996633),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 9,
		fgColor = rgb(0x007F7F),
		bgColor = defaultBg,
	},
	["DOUBLE QUOTED STRING"] = {
		id = 10,
		fgColor = rgb(0xCC3300),
		bgColor = defaultBg,
	},
	["LOCALE"] = {
		id = 11,
		fgColor = rgb(0x0000CC),
		bgColor = defaultBg,
	},
}
return L
