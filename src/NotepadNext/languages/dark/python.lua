local L = {}

L.lexer = "python"

L.singleLineComment = "# "

L.tabSize = 4

L.tabSettings = "spaces"

L.first_line = {
	"^#!.*python"
}

L.extensions = {
	"py",
	"pyw",
}

L.properties = {
    ["lexer.python.keywords2.no.sub.identifiers"] = "1"
}

L.keywords = {
	[0] = "and as assert async await break class continue def del elif else except finally for from global if import in is lambda nonlocal not or pass raise return try while with yield self",
	[1] = "ArithmeticError AssertionError AttributeError BaseException BlockingIOError BrokenPipeError BufferError BytesWarning ChildProcessError ConnectionAbortedError ConnectionError ConnectionRefusedError ConnectionResetError DeprecationWarning EOFError Ellipsis EnvironmentError Exception False FileExistsError FileNotFoundError FloatingPointError FutureWarning GeneratorExit IOError ImportError ImportWarning IndentationError IndexError InterruptedError IsADirectoryError KeyError KeyboardInterrupt LookupError MemoryError ModuleNotFoundError NameError None NotADirectoryError NotImplemented NotImplementedError OSError OverflowError PendingDeprecationWarning PermissionError ProcessLookupError RecursionError ReferenceError ResourceWarning RuntimeError RuntimeWarning StopAsyncIteration StopIteration SyntaxError SyntaxWarning SystemError SystemExit TabError TimeoutError True TypeError UnboundLocalError UnicodeDecodeError UnicodeEncodeError UnicodeError UnicodeTranslateError UnicodeWarning UserWarning ValueError Warning WindowsError ZeroDivisionError abs all any ascii bin bool breakpoint bytearray bytes callable chr classmethod compile complex copyright credits delattr dict dir divmod enumerate eval exec exit filter float format frozenset getattr globals hasattr hash help hex id input int isinstance issubclass iter len license list locals map max memoryview min next object oct open ord pow print property quit range repr reversed round set setattr slice sorted staticmethod str sum super tuple type vars zip",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENTLINE"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0xFF0000),
		bgColor = darkBg,
	},
	["STRING"] = {
		id = 3,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["CHARACTER"] = {
		id = 4,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["KEYWORDS"] = {
		id = 5,
		fgColor = rgb(cppInstructionColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["BUILTINS"] = {
		id = 14,
		fgColor = rgb(0x880088),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["TRIPLE"] = {
		id = 6,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["TRIPLEDOUBLE"] = {
		id = 7,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["CLASSNAME"] = {
		id = 8,
		fgColor = darkFg,
		bgColor = darkBg,
		fontStyle = 1,
	},
	["DEFNAME"] = {
		id = 9,
		fgColor = rgb(0xFF00FF),
		bgColor = darkBg,
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(cppOperatorColor),
		bgColor = darkBg,
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 11,
		fgColor = darkFg,
		bgColor = darkBg,
	},
	["COMMENTBLOCK"] = {
		id = 12,
		fgColor = rgb(0x008000),
		bgColor = darkBg,
	},
	["DECORATOR"] = {
		id = 15,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
		fontStyle = 2,
	},
	["F STRING"] = {
		id = 16,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["F CHARACTER"] = {
		id = 17,
		fgColor = rgb(0x808080),
		bgColor = darkBg,
	},
	["F TRIPLE"] = {
		id = 18,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
	["F TRIPLEDOUBLE"] = {
		id = 19,
		fgColor = rgb(0xFF8000),
		bgColor = darkBg,
	},
}
return L
