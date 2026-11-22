local L = {}

-- CUE sheets have no dedicated Lexilla lexer. The "conf" lexer already
-- handles quoted strings, numbers, punctuation and two configurable word
-- lists, which maps well onto CUE commands and their arguments.
--
-- LexConf lowercases every identifier before the WordList lookup
-- (LexConf.cxx:86,138), so keywords here MUST be lowercase. It also only
-- starts an identifier on isalpha() (LexConf.cxx:83), which is why "4ch"
-- (a FLAGS value) cannot be listed.
L.lexer = "conf"

L.singleLineComment = "REM "

L.extensions = {
    "cue",
}

L.keywords = {
    -- SCE_CONF_DIRECTIVE (9): CUE commands
    [0] = "arranger catalog cdtextfile composer file flags index isrc message performer postgap pregap rem songwriter title track",

    -- SCE_CONF_PARAMETER (5): REM fields, FILE types and FLAGS values
    [1] = "aiff audio binary cdg comment data date dcp discid fps genre mode1 mode1_raw mode2 mode2_raw motorola mp3 pre replaygain_album_gain replaygain_track_gain scms wave",
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
    ["IDENTIFIER"] = {
        id = 3,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
    },
    ["EXTENSION"] = {
        id = 4,
        fgColor = rgb(0x8000FF),
        bgColor = rgb(0xFFFFFF),
    },
    ["PARAMETER"] = {
        id = 5,
        fgColor = rgb(0x008080),
        bgColor = rgb(0xFFFFFF),
    },
    ["STRING"] = {
        id = 6,
        fgColor = rgb(0x808080),
        bgColor = rgb(0xFFFFFF),
    },
    ["OPERATOR"] = {
        id = 7,
        fgColor = rgb(0x000080),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["IP"] = {
        id = 8,
        fgColor = rgb(0xFF8000),
        bgColor = rgb(0xFFFFFF),
    },
    ["DIRECTIVE"] = {
        id = 9,
        fgColor = rgb(0x0000FF),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
}

return L
