local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
    "js",
    "mjs",
    "cjs",
    "jsm",
    "jsx",
    "ts",
    "tsx",
}

L.properties = {
    ["lexer.cpp.backquoted.strings"] = "1",
}

L.keywords = {
    [0] = "abstract async await boolean break byte case catch char class const continue debugger default delete do double else enum export extends final finally float for function goto if implements import in instanceof int interface let long native new null of package private protected public return short static super switch synchronized this throw throws transient try typeof var void volatile while with true false prototype yield from as",
    [1] = "Array Date eval hasOwnProperty Infinity isFinite isNaN isPrototypeOf Math NaN Number Object prototype String toString undefined valueOf",
    [3] = "alert all anchor anchors area assign blur button checkbox clearInterval clearTimeout clientInformation close closed confirm constructor crypto decodeURI decodeURIComponent defaultStatus document element elements embed embeds encodeURI encodeURIComponent escape event fileUpload focus form forms frame innerHeight innerWidth layer layers link location mimeTypes navigate navigator frames frameRate hidden history image images offscreenBuffering onblur onclick onerror onfocus onkeydown onkeypress onkeyup onmouseover onload onmouseup onmousedown onsubmit open opener option outerHeight outerWidth packages pageXOffset pageYOffset parent parseFloat parseInt password pkcs11 plugin prompt propertyIsEnum radio reset screenX screenY scroll secure select self setInterval setTimeout status submit taint text textarea top unescape untaint window",
}

L.styles = {
    ["WHITE SPACE"] = {
        id = 0,
        fgColor = rgb(defaultFg),
        bgColor = rgb(defaultBg),
    },
    ["DEFAULT"] = {
        id = 11,
        fgColor = rgb(defaultFg),
        bgColor = rgb(defaultBg),
    },
    ["INSTRUCTION WORD"] = {
        id = 5,
        fgColor = rgb(InstructionColor),
        bgColor = rgb(defaultBg),
        fontStyle = 1,
    },
    ["TYPE WORD"] = {
        id = 16,
        fgColor = rgb(TypeColor),
        bgColor = rgb(defaultBg),
    },
    ["WINDOW INSTRUCTION"] = {
        id = 19,
        fgColor = rgb(0x804000),
        bgColor = rgb(defaultBg),
        fontStyle = 1,
    },
    ["NUMBER"] = {
        id = 4,
        fgColor = rgb(0xFF8000),
        bgColor = rgb(defaultBg),
    },
    ["STRING"] = {
        id = 6,
        fgColor = rgb(0x808080),
        bgColor = rgb(defaultBg),
    },
    ["STRINGRAW"] = {
        id = 20,
        fgColor = rgb(OperatorColor),
        bgColor = rgb(0xC0C0C0),
    },
    ["CHARACTER"] = {
        id = 7,
        fgColor = rgb(0x808080),
        bgColor = rgb(defaultBg),
    },
    ["OPERATOR"] = {
        id = 10,
        fgColor = rgb(OperatorColor),
        bgColor = rgb(defaultBg),
        fontStyle = 1,
    },
    ["VERBATIM"] = {
        id = 13,
        fgColor = rgb(defaultFg),
        bgColor = rgb(defaultBg),
    },
    ["REGEX"] = {
        id = 14,
        fgColor = rgb(defaultFg),
        bgColor = rgb(defaultBg),
        fontStyle = 1,
    },
    ["COMMENT"] = {
        id = 1,
        fgColor = rgb(0x008000),
        bgColor = rgb(defaultBg),
    },
    ["COMMENT LINE"] = {
        id = 2,
        fgColor = rgb(0x008000),
        bgColor = rgb(defaultBg),
    },
    ["COMMENT DOC"] = {
        id = 3,
        fgColor = rgb(0x008080),
        bgColor = rgb(defaultBg),
    },
    ["COMMENT LINE DOC"] = {
        id = 15,
        fgColor = rgb(0x008080),
        bgColor = rgb(defaultBg),
    },
    ["COMMENT DOC KEYWORD"] = {
        id = 17,
        fgColor = rgb(0x008080),
        bgColor = rgb(defaultBg),
        fontStyle = 1,
    },
    ["COMMENT DOC KEYWORD ERROR"] = {
        id = 18,
        fgColor = rgb(0x008080),
        bgColor = rgb(defaultBg),
    },
}
return L
