local L = {}

L.lexer = "cobol"

L.extensions = {
	"cbl",
	"cbd",
	"cdb",
	"cdc",
	"cob",
	"cpy",
	"copy",
	"lst",
}

L.keywords = {
	[0] = "configuration data declaratives division environment file file-control function function-id i-o i-o-control identification input input-output linkage local-storage output procedure program program-id screen section special-names working-storage exec end-exec",
	[1] = "accept add allocate alter call cancel close commit compute continue delete destroy disable display divide enable entry evaluate also when exit free generate go to goback if else initialize initiate inquire invoke inspect merge modify move multiply next sentence open perform purge raise read ready receive release reset resume return rewrite rollback search send set sort start stop string subtract suppress terminate transform unlock unstring use validate write   end-accept end-add end-call end-compute end-delete end-display end-divide end-evaluate end-if end-multiply end-of-page end-perform end-read end-receive end-return end-rewrite end-search end-start end-string end-subtract end-unstring end-write   copy replace   abs acos annuity asin atan boolean-of-integer byte-length char char-national combined-datetime concatenate cos currency-symbol current-date date-of-integer date-to-yyyymmdd day-of-integer day-to-yyyyddd display-of e exception-file exception-file-n exception-location exception-location-n exception-statement exception-status exp exp10 factorial formatted-current-date formatted-date formatted-datetime formatted-time fraction-part highest-algebraic integer integer-of-boolean integer-of-date integer-of-day integer-of-formatted-date integer-part length length-an locale-compare locale-date locale-time locale-time-from-seconds log log10 lower-case lowest-algebraic max mean median midrange min mod module-caller-id module-date module-formatted-date module-id module-path module-source module-time monetary-decimal-point monetary-thousands-separator national-of numeric-decimal-point numeric-thousands-separator numval numval-c numval-f ord ord-max ord-min pi present-value random range rem reverse seconds-from-formatted-time seconds-past-midnight sign sin sqrt standard-compare standard-deviation stored-char-length substitute substitute-case sum tan test-date-yyyymmdd test-day-yyyyddd test-formatted-datetime test-numval test-numval-c test-numval-f trim upper-case variance when-compiled year-to-yyyy",
	[2] = "high-value high-values low-value low-values space spaces quote quotes zero zeroes zeros  author date-compiled date-modified date-written installation remarks security   cob-crt-status debug-item debug-line debug-name debug-sub-1 debug-sub-2 debug-sub-3 debug-contents number-of-call-parameters return-code return-code-unsigned sort-return  tally dialect-all jnienvptr json-code linage-counter shift-in shift-out sort-control sort-core-size sort-file-size sort-message sort-mode-size  xml-code xml-event xml-information xml-namespace xml-namespace-prefix xml-nnamespace xml-nnamespace-prefix xml-ntext xml-text  sysin sysipt stdin sysout syslist syslst stdout print printer printer-1 syserr stderr console c01 c02 c03 c04 c05 c06 c07 c08 c09 c10 c11 c12 csp formfeed call-convention  switch-0 switch-1 switch-2 switch-3 switch-4 switch-5 switch-6 switch-7 switch-8 switch-9 switch-10 switch-11 switch-12 switch-13 switch-14 switch-15 switch-16 switch-17 switch-18  switch-19 switch-20 switch-21 switch-22 switch-23 switch-24 switch-25 switch-26 switch-27 switch-28 switch-29 switch-30 switch-31 switch-32 switch-33 switch-34 switch-35 switch-36  upsi-0 upsi-1 upsi-2 upsi-3 upsi-4 upsi-5 upsi-6 upsi-7  access active-class address advancing after aligned all alphabet alphabetic alphabetic-lower alphabetic-upper alphanumeric alphanumeric-edited alternate and any anycase are area areas as ascending assign at automatic  b-and b-not b-or b-xor based before binary binary-char binary-double binary-long binary-sequential binary-short bit blank block boolean bottom by  cd cf ch character characters class class-id clock-units code-set col collating cols column columns comma common communication comp comp-3 comp-5 computational computational-3 computational-5 condition constant contains content control controls converting corr corresponding count crt currency cursor  data-pointer date day day-of-week de debugging decimal-point default delimited delimiter depending descending destination detail down duplicates dynamic ec egi emi end enter eo eop equal equals error esi every exception exception-object exclusive extend external  factory false farthest-from-zero fd filler final first float-binary-128 float-binary-32 float-binary-64 float-decimal-16 float-decimal-34 float-extended float-infinity float-long float-not-a-number-quiet float-not-a-number-signaling float-short footing for format from function-pointer  get giving global greater group group-usage heading in in-arithmetic-range index indexed indicate inherits initial initialized interface interface-id into invalid is just justified key label last leading left less limit limits linage line line-counter line-sequential lines locale lock  manual message method method-id minus mode modules multiple national national-edited native nearest-to-zero negative negative-infinity nested no not null number numbers numeric numeric-edited object object-computer object-reference occurs of off omitted on only optional options or order organization other overflow override  packed-decimal padding page page-counter pf ph physical pic picture plus pointer position positive positive-infinity present previous printing procedures proceed program-pointer property prototype queue  raising rd record records redefines reel reference references relative remainder removal renames replacing report reporting reports repository rerun reserve retry returning reversed rewind rf rh right rounded run  same sd segment segment-limit select self separate sequence sequential sharing signed size sort-merge source source-computer sources standard standard-1 standard-2 status step sub-queue-1 sub-queue-2 sub-queue-3 super symbolic sync synchronized system-default  table tallying terminal test text than then through thru time times top trailing true type typedef unit universal unsigned until up upon usage user-default using val-status valid validate-status value values varying with words  arithmetic attribute auto away-from-zero background-color bell binary-c-long binary-encoding blink capacity center chaining classification cobol comp-1 comp-4 comp-6 comp-x computational-1 computational-4 computational-6 computational-x cycle  decimal-encoding entry-convention eol eos erase expands float-binary float-decimal float-not-a-number foreground-color forever full high-order-left high-order-right highlight ignoring implements intermediate intrinsic lc_all lc_collate lc_ctype lc_messages lc_monetary lc_numeric lc_time lowlight  memory nearest-away-from-zero nearest-even-intermediate nearest-toward-zero none normal  paragraph prefixed prohibited recursive relation required reverse-video rounding seconds secure short standard-binary standard-decimal statement strong structure symbol tape toward-greater toward-lesser truncation ucs-4 underline utf-16 utf-8 yyyyddd yyyymmdd  3-d apply background background-high background-low bar blob bold box busy cbl clob combo-box dbclob dbcs draw drop grid grip high hot-track hscroll hscroll-pos id imp kanji low num-rows o-fill oostackptr pos prompt public rowid scroll scroll-bar shadow skip1 skip2 skip3 sql  tab track-thumb unbounded variant virtual-width vscroll vscroll-bar vscroll-pos wait width wrap xml",
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
	["DECLARATION"] = {
		id = 5,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["INSTRUCTION WORD"] = {
		id = 16,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORD"] = {
		id = 8,
		fgColor = rgb(0x0080FF),
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
}
return L
