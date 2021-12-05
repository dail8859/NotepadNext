local L = {}

L.lexer = "markdown"

L.extensions = {
    "md",
    "markdown",
}

L.styles = {
    ["DEFAULT"] = {
        id = 0,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
    },
    ["SCE_MARKDOWN_STRONG1"] = {
        id = 2,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_STRONG2"] = {
        id = 3,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_EM1"] = {
        id = 4,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 2,
    },
    ["SCE_MARKDOWN_EM2"] = {
        id = 5,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 2,
    },
    ["SCE_MARKDOWN_HEADER1"] = {
        id = 6,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER2"] = {
        id = 7,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER3"] = {
        id = 8,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER4"] = {
        id = 9,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER5"] = {
        id = 10,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER6"] = {
        id = 11,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_PRECHAR"] = {
        id = 12,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xEEEEAA),
    },
    ["SCE_MARKDOWN_ULIST_ITEM"] = {
        id = 13,
        fgColor = rgb(0x555555),
        bgColor = rgb(0xFFFFFF),
    },
    ["SCE_MARKDOWN_OLIST_ITEM"] = {
        id = 14,
        fgColor = rgb(0x555555),
        bgColor = rgb(0xFFFFFF),
    },
    ["SCE_MARKDOWN_OLIST_ITEM"] = {
        id = 15,
        fgColor = rgb(0x000088),
        bgColor = rgb(0xFFFFFF),
    },
    ["SCE_MARKDOWN_STRIKEOUT"] = {
        id = 16,
        fgColor = rgb(0x18453B),
        bgColor = rgb(0xA9BA9D),
    },
    ["SCE_MARKDOWN_HRULE"] = {
        id = 17,
        fgColor = rgb(0x555555),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_LINK"] = {
        id = 18,
        fgColor = rgb(0x0000AA),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 4,
    },
    ["SCE_MARKDOWN_CODE"] = {
        id = 19,
        fgColor = rgb(0x000088),
        bgColor = rgb(0xEEEEEE),
    },
    ["SCE_MARKDOWN_CODE2"] = {
        id = 20,
        fgColor = rgb(0x000088),
        bgColor = rgb(0xEEEEEE),
    },
    ["SCE_MARKDOWN_CODEBK"] = {
        id = 20,
        fgColor = rgb(0x000088),
        bgColor = rgb(0xEEEEEE),
    },
}

return L
