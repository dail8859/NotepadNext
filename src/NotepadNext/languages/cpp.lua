local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"cpp",
	"cxx",
	"cc",
	"h",
	"hh",
	"hpp",
	"hxx",
	"ino",
}

L.properties = {
	["fold.cpp.comment.explicit"] = "0",
	["lexer.cpp.track.preprocessor"] = "0",
	["lexer.cpp.escape.sequence"] = "1",
}

L.keywords = {
	[0] = "alignof and and_eq bitand bitor break case catch compl const_cast continue default delete do dynamic_cast else false for goto if namespace new not not_eq nullptr operator or or_eq reinterpret_cast return sizeof static_assert static_cast switch this throw true try typedef typeid using while xor xor_eq NULL",
	[1] = "alignas asm auto bool char char16_t char32_t class clock_t concept const consteval constexpr constinit decltype double enum explicit export extern final float friend inline int int8_t int16_t int32_t int64_t int_fast8_t int_fast16_t int_fast32_t int_fast64_t intmax_t intptr_t long mutable noexcept override private protected ptrdiff_t public register requires short signed size_t ssize_t static struct template thread_local time_t typename uint8_t uint16_t uint32_t uint64_t uint_fast8_t uint_fast16_t uint_fast32_t uint_fast64_t uintmax_t uintptr_t union unsigned virtual void volatile wchar_t",
	[2] = "a addindex addtogroup anchor arg attention author authors b brief bug c callergraph callgraph category cite class code cond copybrief copydetails copydoc copyright date def defgroup deprecated details diafile dir docbookonly dontinclude dot dotfile e else elseif em endcode endcond enddocbookonly enddot endhtmlonly endif endinternal endlatexonly endlink endmanonly endmsc endparblock endrtfonly endsecreflist enduml endverbatim endxmlonly enum example exception extends f$ f[ f] file fn f{ f} headerfile hidecallergraph hidecallgraph hideinitializer htmlinclude htmlonly idlexcept if ifnot image implements include includelineno ingroup interface internal invariant latexinclude latexonly li line link mainpage manonly memberof msc mscfile n name namespace nosubgrouping note overload p package page par paragraph param parblock post pre private privatesection property protected protectedsection protocol public publicsection pure ref refitem related relatedalso relates relatesalso remark remarks result return returns retval rtfonly sa secreflist section see short showinitializer since skip skipline snippet startuml struct subpage subsection subsubsection tableofcontents test throw throws todo tparam typedef union until var verbatim verbinclude version vhdlflow warning weakgroup xmlonly xrefitem",
	[5] = "FIXME TODO XXX HACK NOTE",
}

L.styles = {
	["WHITE SPACE"] = {
		id = 0,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["PREPROCESSOR"] = {
		id = 9,
		fgColor = rgb(0x804000),
		bgColor = defaultBg,
	},
	["DEFAULT"] = {
		id = 11,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(InstructionColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["TYPE WORD"] = {
		id = 16,
		fgColor = rgb(TypeColor),
		bgColor = defaultBg,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = defaultBg,
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = defaultBg,
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(OperatorColor),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["VERBATIM"] = {
		id = 13,
		fgColor = defaultFg,
		bgColor = defaultBg,
	},
	["REGEX"] = {
		id = 14,
		fgColor = defaultFg,
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
	["COMMENT DOC KEYWORD"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 18,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
	["PREPROCESSOR COMMENT"] = {
		id = 23,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
	},
	["PREPROCESSOR COMMENT DOC"] = {
		id = 24,
		fgColor = rgb(0x008080),
		bgColor = defaultBg,
	},
	["TASK MARKER"] = {
		id = 26,
		fgColor = rgb(0x008000),
		bgColor = defaultBg,
		fontStyle = 1,
	},
}
return L
