local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
    "go",
}

L.properties = {
    ["fold.cpp.comment.explicit"] = "0",
    ["lexer.cpp.track.preprocessor"] = "0",
    ["lexer.cpp.escape.sequence"] = "1",
}

L.keywords = {
    [0] = "bool byte break case chan complex64 complex128 const continue defer default else for func go goto fallthrough false float32 float64 if import int interface int8 int16 int32 int64 len map nil package range return select string struct switch true type uint uintptr uint8 uint16 uint32 uint64 var",
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
