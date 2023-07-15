local L = {}

L.lexer = "mmixal"

L.singleLineComment = "# "

L.extensions = {
	"mms",
}

L.keywords = {
	[0] = "2ADDU 4ADDU 8ADDU 16ADDU ADD ADDU AND ANDNH ANDNL ANDNMH ANDNML BDIF BEV BN BNN BNP BNZ BOD BP BSPEC BYTE BZ CMP CMPU CSEV CSN CSNN CSNP CSNZ CSOD CSP CSWAP CSZ DIV DIVU ESPEC EXPR FADD FCMP FCMPE FDIV FEQL FEQLE FIX FIXU FLOT FLOTU FMUL FREM FSQRT FSUB FUN FUNE GET GETA GO GREG I_BIT INCH INCL INCMH INCML IS JMP LDA LDB LDBU LDHT LDO LDOU LDSF LDT LDTU LDUNC LDVTS LDW LDWU LOC LOCAL MOR MUL MULU MUX MXOR NAND NEG NEGU NNIX NOR NXOR O_BIT OCTA ODIF OR ORH ORL ORMH ORML ORN PBEV PBN PBNN PBNP PBNZ PBOD PBP PBZ POP PREFIX PREGO PRELD PREST PUSHGO PUSHJ PUT RESUME SAVE SET SETH SETL SETMH SETML SFLOT SFLOTU SL SLU SR SRU STB STBU STCO STHT STO STOU STSF STT STTU STUNC STW STWU SUB SUBU SWYM SYNC SYNCD TDIF TETRA TRAP TRIP UNSAVE WDIF WYDEXOR ZSEV ZSN ZSNN ZSNP ZSNZ ZSOD ZSP ZSZ",
	[1] = "rA rB rC rD rE rF rG rH rI rJ rK rL rM rN rO rP rQ rR rS rT rU rV rW rX rY rZ rBB rTT rWW rXX rYY rZZ",
	[2] = "@ Text_Segment Data_Segment Pool_Segment Stack_Segment StdErr StdIn StdOut Fopen Fclose Fread Fwrite Fgets Fputs Fgetws Fputws Ftell Fseek TextRead TextWrite BinaryRead BinaryWrite BinaryReadWrite",
}

L.styles = {
	["DIVSION OF LEADING WHITESPACE IN LINE"] = {
		id = 0,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = rgb(defaultBg),
	},
	["LABEL"] = {
		id = 2,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["OPCODE"] = {
		id = 3,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["DIVISION BETWEEN LABEL AND OPCODE"] = {
		id = 4,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["VALID OPCODE"] = {
		id = 5,
		fgColor = rgb(0x00007F),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["UNKNOWN OPCODE"] = {
		id = 6,
		fgColor = rgb(0xFFFF00),
		bgColor = rgb(0xFF0000),
	},
	["DIVISION BETWEEN OPCODE AND OPERANDS"] = {
		id = 7,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["DIVISION OF OPERANDS"] = {
		id = 8,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["NUMBER"] = {
		id = 9,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(defaultBg),
	},
	["REFERENCE (TO A LABEL)"] = {
		id = 10,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
	},
	["CHAR"] = {
		id = 11,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(defaultBg),
	},
	["STRING"] = {
		id = 12,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(defaultBg),
	},
	["REGISTER"] = {
		id = 13,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(defaultBg),
	},
	["HEXADECIMAL NUMBER"] = {
		id = 14,
		fgColor = rgb(0x7F0000),
		bgColor = rgb(defaultBg),
	},
	["OPERATOR"] = {
		id = 15,
		fgColor = rgb(defaultFg),
		bgColor = rgb(defaultBg),
		fontStyle = 1,
	},
	["SYMBOL"] = {
		id = 16,
		fgColor = rgb(0xFF7777),
		bgColor = rgb(defaultBg),
	},
	["COMMENT OTHERWISE"] = {
		id = 17,
		fgColor = rgb(0x007F00),
		bgColor = rgb(defaultBg),
	},
}
return L
