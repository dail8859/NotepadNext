local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"c",
	"lex",
}

L.keywords = {
	[0] = "if else switch case default break goto return for while do continue typedef sizeof offsetof alignof _Alignof _Generic _Static_assert and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq NULL",
	[1] = "void struct union enum char short int long double float signed unsigned const static extern auto register volatile restrict _Atomic bool _Bool complex _Complex imaginary _Imaginary inline noreturn _Noreturn alignas _Alignas thread_local _Thread_local uint8_t uint16_t uint32_t uint64_t int8_t int16_t int32_t int64_t uint_least8_t uint_least16_t uint_least32_t uint_least64_t int_least8_t int_least16_t int_least32_t int_least64_t uint_fast8_t uint_fast16_t uint_fast32_t uint_fast64_t int_fast8_t int_fast16_t int_fast32_t int_fast64_t uintptr_t intptr_t uintmax_t intmax_t ptrdiff_t max_align_t div_t ldiv_t lldiv_t imaxdiv_t size_t time_t clock_t wchar_t char16_t char32_t sig_atomic_t FILE",
}

L.styles = {
	["PREPROCESSOR"] = {
		id = 9,
		fgColor = rgb(0x804000),
		bgColor = rgb(0xFFFFFF),
	},
	["DEFAULT"] = {
		id = 11,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["TYPE WORD"] = {
		id = 16,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["VERBATIM"] = {
		id = 13,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["REGEX"] = {
		id = 14,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC KEYWORD"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 18,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROCESSOR COMMENT"] = {
		id = 23,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["PREPROCESSOR COMMENT DOC"] = {
		id = 24,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
