local L = {}

-- Dedicated CUE lexer: thirdparty/lexilla/lexers/LexCue.cxx.
-- The five word lists mirror the six rule groups of the CotEditor
-- "CUE Sheet" .cotsyntax bundle (the sixth group, numbers, is handled
-- by the lexer itself so that a MM:SS:FF timecode stays one styled run).
--
-- The lexer lowercases every token before the WordList lookup, so all
-- keywords here MUST be lower case. Unlike the generic "conf" lexer it
-- accepts digit-leading tokens, which is why "4ch" can be listed.
L.lexer = "cue"

L.extensions = {
    "cue",
}

L.keywords = {
    -- SCE_CUE_KEYWORD (3): commands that open a line
    [0] = "file track index pregap postgap",

    -- SCE_CUE_COMMAND (4): the other line-opening commands
    [1] = "arranger catalog cdtextfile composer flags isrc message performer rem songwriter title",

    -- SCE_CUE_ATTRIBUTE (5): fields immediately following REM
    [2] = "comment date discid genre replaygain_album_gain replaygain_track_gain",

    -- SCE_CUE_TYPE (6): FILE formats and TRACK modes
    [3] = "aiff audio binary cdg mode1 mode1_raw mode2 mode2_raw motorola mp3 wave",

    -- SCE_CUE_VALUE (7): FLAGS values
    [4] = "4ch data dcp fps pre scms",
}

L.styles = {
    ["DEFAULT"] = {
        id = 0,
        fgColor = rgb(0x1A1A1A),
        bgColor = rgb(0xFFFFFF),
    },
    ["STRING"] = {
        id = 1,
        fgColor = rgb(0x33475B),
        bgColor = rgb(0xFFFFFF),
    },
    ["NUMBER"] = {
        id = 2,
        fgColor = rgb(0xC25E00),
        bgColor = rgb(0xFFFFFF),
    },
    ["KEYWORD"] = {
        id = 3,
        fgColor = rgb(0x0033CC),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["COMMAND"] = {
        id = 4,
        fgColor = rgb(0x7A1FA2),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["ATTRIBUTE"] = {
        id = 5,
        fgColor = rgb(0x00838F),
        bgColor = rgb(0xFFFFFF),
    },
    ["TYPE"] = {
        id = 6,
        fgColor = rgb(0x1B7F3B),
        bgColor = rgb(0xFFFFFF),
    },
    ["VALUE"] = {
        id = 7,
        fgColor = rgb(0xB01B6B),
        bgColor = rgb(0xFFFFFF),
    },
    ["IDENTIFIER"] = {
        id = 8,
        fgColor = rgb(0x1A1A1A),
        bgColor = rgb(0xFFFFFF),
    },
}

return L
