local L = {}

L.name = "Text"

L.lexer = "null"

L.disableFoldMargin = true

L.extensions = {
    "",
    "txt",
}

L.styles = {
    ["DEFAULT"] = {
        id = 0,
        fgColor = defaultFg,
        bgColor = defaultBg,
        fontStyle = 0,
    },
}

return L
