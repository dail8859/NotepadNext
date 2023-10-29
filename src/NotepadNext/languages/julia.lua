local L = {}

L.lexer = "julia"

L.singleLineComment = "# "

L.extensions = {
    "jl",
}

-- "Primary keywords and identifiers",
-- "Built in types",
-- "Other keywords",
-- "Built in functions",

L.keywords = {
    [0] = "baremodule begin break catch const continue do else elseif end export false finally for function global if import let local macro module quote return struct true try using while",
    [1] = "Number Complex Real AbstractFloat Float16 Float32 Float64 BigFloat Integer Bool Signed Int8 Int16 Int32 Int64 Int128 BigInt Unsigned UInt8 UInt16 UInt32 UInt64 UInt128 Rational AbstractIrrational Irrational",
    [2] = "",
    [3] = ""
}

L.styles = {
    ["DEFAULT"] = {
        id = 0,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
    },
    ["COMMENT"] = {
        id = 1,
        fgColor = rgb(0x008000),
        bgColor = rgb(0xFFFFFF),
    },
    ["NUMBER"] = {
        id = 2,
        fgColor = rgb(0xFF8000),
        bgColor = rgb(0xFFFFFF),
    },
    ["KEYWORD1"] = {
        id = 3,
        fgColor = rgb(0x0000FF),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["KEYWORD2"] = {
        id = 4,
        fgColor = rgb(0x8000FF),
        bgColor = rgb(0xFFFFFF),
    },
    ["KEYWORD3"] = {
        id = 5,
        fgColor = rgb(0x8000FF),
        bgColor = rgb(0xFFFFFF),
    },
    ["CHARACTER"] = {
        id = 6,
        fgColor = rgb(0x808080),
        bgColor = rgb(0xFFFFFF),
    },
    -- Operator
    -- Bracket
    -- identifier
    ["STRING"] = {
        id = 10,
        fgColor = rgb(0x808080),
        bgColor = rgb(0xFFFFFF),
    },
    -- Symbol
    ["MACRO"] = {
        id = 12,
        fgColor = rgb(0x804000),
        bgColor = rgb(0xFFFFFF),
    },
    ["STRINGINTERP"] = {
        id = 13,
        fgColor = rgb(0x804000),
        bgColor = rgb(0xFFFFFF),
    },
    ["DOC STRING"] = {
        id = 14,
        fgColor = rgb(0x008080),
        bgColor = rgb(0xFFFFFF),
    },
    ["STRING LITERAL"] = {
        id = 15,
        fgColor = rgb(0x808080),
        bgColor = rgb(0xFFFFFF),
    },
    -- Command
    -- Command Literal
    -- Type annotation identifier
    -- LEXERROR
    -- KEYWORD4
    -- TYPEOPERATOR
}

return L
