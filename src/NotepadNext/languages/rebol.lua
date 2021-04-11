local L = {}

L.lexer = "rebol"

L.extensions = {
	"r2",
	"r3",
	"reb",
}

L.keywords = {
	[0] = "about abs absolute add alert alias all alter and any any-block? any-function? any-string? any-type? any-word? append arccosine arcsine arctangent array as-pair ask at attempt back binary? bind bitset? block? break brightness? browse build-tag caret-to-offset catch center-face change change-dir char? charset checksum choose clean-path clear clear-fields close comment complement component? compose compress confirm connected? construct context copy cosine datatype? date? debase decimal? decode-cgi decompress dehex delete detab difference dir? dirize disarm dispatch divide do do-events does dump-face dump-obj echo either else email? empty? enbase entab equal? error? even? event? exclude exists? exit exp extract fifth file? find first flash focus for forall foreach forever form forskip found? fourth free func function function? get get-modes get-word? greater-or-equal? greater? halt has hash? head head? help hide hide-popup if image? import-email in in-window? index? info? inform input input? insert integer? intersect issue? join last launch layout length? lesser-or-equal? lesser? library? license link? list-dir list? lit-path? lit-word? load load-image log-10 log-2 log-e logic? loop lowercase make make-dir make-face max maximum maximum-of min minimum minimum-of modified? mold money? multiply native? negate negative? next none? not not-equal? now number? object? odd? offset-to-caret offset? op? open or pair? paren? parse parse-xml path? pick poke port? positive? power prin print probe protect protect-system query quit random read read-io recycle reduce refinement? reform rejoin remainder remold remove remove-each rename repeat repend replace request request-color request-date request-download request-file request-list request-pass request-text resend return reverse routine? same? save script? second secure select send series? set set-modes set-net set-path? set-word? show show-popup sign? sine size-text size? skip sort source span? split-path square-root strict-equal? strict-not-equal? string? struct? stylize subtract suffix? switch tag? tail tail? tangent third throw time? to to-binary to-bitset to-block to-char to-date to-decimal to-email to-file to-get-word to-hash to-hex to-idate to-image to-integer to-issue to-list to-lit-path to-lit-word to-local-file to-logic to-money to-pair to-paren to-path to-rebol-file to-refinement to-set-path to-set-word to-string to-tag to-time to-tuple to-url to-word trace trim try tuple? type? unfocus union unique unprotect unset unset? until unview update upgrade uppercase url? usage use value? view viewed? wait what what-dir while within? word? write write-io xor zero?",
	[1] = "action? any-block? any-function? any-string? any-type? any-word? binary? bitset? block? char? component? connected? datatype? date? decimal? dir? email? empty? equal? error? even? event? exists? file? found? function? get-word? greater-or-equal greater? hash? head? image? index? info? input? integer? issue? length? lesser-or-equal? lesser? library? link-app? link? list? lit-path? lit-word? logic? modified? money? native? negative? none? not-equal? number? object? odd? offset? op? pair? paren? path? port? positive? rebol-command? rebol-encap? rebol-link? rebol-pro? rebol-view? refinement? routine? same? script? series? set-path? set-word? sign? size? strict-equal? strict-not-equal string? struct? suffix? tag? tail? time? tuple? type? unset? url? value? view? word? zero?",
	[2] = "action! any-block! any-function! any-string! any-type!  any-word! binary! bitset! block! char! datatype! date! decimal! email! error!  event! file! function! get-word! hash! image! integer! issue! library! list! lit-path!  lit-word! logic! money! native! none! number! object! op! pair! paren! path!  port! refinement! routine! series! set-path! set-word! string! struct! symbol! tag!  time! tuple! unset! url! word!",
	[3] = "None",
	[4] = "None",
	[5] = "None",
	[6] = "None",
}

L.styles = {
	["DEFAULT"] = {
		id = 32,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["ANY OTHER TEXT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["LINE COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
	},
	["BLOCK COMMENT"] = {
		id = 2,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
	},
	["PREFACE"] = {
		id = 3,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["OPERATORS"] = {
		id = 4,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["CHARACTERS"] = {
		id = 5,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING WITH QUOTES"] = {
		id = 6,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING WITH BRACES"] = {
		id = 7,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 8,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
	},
	["PAIR ( 800X600 )"] = {
		id = 9,
		fgColor = rgb(0x005090),
		bgColor = rgb(0xFFFFFF),
	},
	["TUPLE ( 127.0.0.1 )"] = {
		id = 10,
		fgColor = rgb(0x005090),
		bgColor = rgb(0xFFFFFF),
	},
	["BINARY ( 16#{1A803F59} )"] = {
		id = 11,
		fgColor = rgb(0xC000C0),
		bgColor = rgb(0xFFFFFF),
	},
	["MONEY"] = {
		id = 12,
		fgColor = rgb(0x003070),
		bgColor = rgb(0xFFFFFF),
	},
	["ISSUE { #123-CD-456 }"] = {
		id = 13,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
	},
	["TAG { <TITLE HEIGHT=100> }"] = {
		id = 14,
		fgColor = rgb(0xC00000),
		bgColor = rgb(0xFFFFFF),
	},
	["FILE { %/C/WINNT/SOME.DLL }"] = {
		id = 15,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xFFFFFF),
	},
	["EMAIL { JOE@MAIL.DOM }"] = {
		id = 16,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xFFFFFF),
	},
	["URL { FTP://THERE.DOM }"] = {
		id = 17,
		fgColor = rgb(0x0080FF),
		bgColor = rgb(0xFFFFFF),
	},
	["DATE { 17-FEB-2004 1/3/99 }"] = {
		id = 18,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["TIME { 12:30 11:22:59 01:59:59.123 }"] = {
		id = 19,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIERS"] = {
		id = 20,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD (ALL)"] = {
		id = 21,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORD (TEST FUNCTIONS)"] = {
		id = 22,
		fgColor = rgb(0x8000C0),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORD (DATATYPES)"] = {
		id = 23,
		fgColor = rgb(0x800060),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORD 4"] = {
		id = 24,
		fgColor = rgb(0xDD9900),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD 5"] = {
		id = 25,
		fgColor = rgb(0xDD9900),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD 6"] = {
		id = 26,
		fgColor = rgb(0xDD9900),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD 7"] = {
		id = 27,
		fgColor = rgb(0xDD9900),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
