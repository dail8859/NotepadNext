function rgb(x)
    return ((x & 0xFF) << 16) | (x & 0xFF00) | ((x & 0xFF0000) >> 16)
end

function DetectLanguageFromContents(contents)
    for name, L in pairs(languages) do
        if L.first_line then
            for _, pattern in ipairs(L.first_line) do
                if string.match(contents, pattern) then
                    return name
                end
            end
        end
    end
    return "Text"
end

function FilterForLanguage(name)
    local extensions = {}
    local language_definition = languages[name]

    if not language_definition.extensions then
        return nil
    end

    for _, ext in ipairs(language_definition.extensions) do
        if #ext > 0 then
            extensions[#extensions + 1] = "*." .. ext
        end
    end

    return  name .. " Files (" .. table.concat(extensions, " ") .. ")"
end

function DialogFilters()
    local filters = {}

    for name, L in pairs(languages) do
        local filter = FilterForLanguage(name)
        if filter then
            filters[#filters + 1] = filter
        end
    end

    table.sort(filters, function (a, b) return a:lower() < b:lower() end)
    table.insert(filters, 1, "All Files (*)")

    return table.concat(filters, ";;")
end

function SetStyle(L)
    if L.styles then
        for _, style in pairs(L.styles) do
            editor.StyleFore[style.id] = style.fgColor
            editor.StyleBack[style.id] = style.bgColor

            if style.fontStyle then
                editor.StyleBold[style.id] = (style.fontStyle & 1 == 1)
                editor.StyleItalic[style.id] = (style.fontStyle & 2 == 2)
                editor.StyleUnderline[style.id] = (style.fontStyle & 4 == 4)
                editor.StyleEOLFilled[style.id] = (style.fontStyle & 8 == 8)
            end
        end
    end

    if L.keywords then
        for id, kw in pairs(L.keywords) do
            editor.KeyWords[id] = kw
        end
    end

    if L.properties then
        for p, v in pairs(L.properties) do
            editor.Property[p] = v
        end
    end
end

function SetLanguage(languageName)
    local L = languages[languageName]

    if not skip_tabs then
        editor.UseTabs = (L.tabSettings or "tabs") == "tabs"
    end

    if not skip_tabwidth then
        editor.TabWidth = L.tabSize or 4
    end

    editor.MarginWidthN[2] = L.disableFoldMargin and 0 or 16

    SetStyle(L)

    if L.additionalLanguages then
        for _, language in pairs(L.additionalLanguages) do
            SetStyle(languages[language])
        end
    end


    editor.Property["fold"] = "1"
    editor.Property["fold.compact"] = "0"
end

languages = {}
languages["ActionScript"] = require("actionscript")
languages["ADA"] = require("ada")
languages["Assembly"] = require("asm")
languages["ASN.1"] = require("asn1")
languages["asp"] = require("asp")
languages["autoIt"] = require("autoit")
languages["AviSynth"] = require("avs")
languages["BaanC"] = require("baanc")
languages["bash"] = require("bash")
languages["Batch"] = require("batch")
languages["BlitzBasic"] = require("blitzbasic")
languages["C"] = require("c")
languages["Caml"] = require("caml")
languages["CMakeFile"] = require("cmake")
languages["COBOL"] = require("cobol")
languages["Csound"] = require("csound")
languages["CoffeeScript"] = require("coffeescript")
languages["C++"] = require("cpp")
languages["C#"] = require("cs")
languages["CSS"] = require("css")
languages["SCSS"] = require("scss")
languages["D"] = require("d")
languages["DIFF"] = require("diff")
languages["Erlang"] = require("erlang")
languages["ESCRIPT"] = require("escript")
languages["Forth"] = require("forth")
languages["Fortran (free form)"] = require("fortran")
languages["Fortran (fixed form)"] = require("fortran77")
languages["FreeBasic"] = require("freebasic")
languages["GUI4CLI"] = require("gui4cli")
languages["Go"] = require("go")
languages["Haskell"] = require("haskell")
languages["HTML"] = require("html")
languages["ini file"] = require("ini")
languages["InnoSetup"] = require("inno")
languages["Intel HEX"] = require("ihex")
languages["Java"] = require("java")
languages["JavaScript"] = require("javascript")
languages["JSON"] = require("json")
languages["KiXtart"] = require("kix")
languages["LISP"] = require("lisp")
languages["LaTeX"] = require("latex")
languages["Lua"] = require("lua")
languages["Less"] = require("less")
languages["Makefile"] = require("makefile")
languages["Markdown"] = require("markdown")
languages["Matlab"] = require("matlab")
languages["MMIXAL"] = require("mmixal")
languages["Nimrod"] = require("nimrod")
languages["Nix"] = require("nix")
languages["extended crontab"] = require("nncrontab")
languages["Dos Style"] = require("nfo")
languages["NSIS"] = require("nsis")
languages["OScript"] = require("oscript")
languages["Objective-C"] = require("objc")
languages["Pascal"] = require("pascal")
languages["Perl"] = require("perl")
languages["PHP"] = require("php")
languages["Postscript"] = require("postscript")
languages["PowerShell"] = require("powershell")
languages["Properties file"] = require("props")
languages["PureBasic"] = require("purebasic")
languages["Python"] = require("python")
languages["R"] = require("r")
languages["REBOL"] = require("rebol")
languages["registry"] = require("registry")
languages["RC"] = require("rc")
languages["Ruby"] = require("ruby")
languages["Rust"] = require("rust")
languages["Scheme"] = require("scheme")
languages["Smalltalk"] = require("smalltalk")
languages["spice"] = require("spice")
languages["SQL"] = require("sql")
languages["S-Record"] = require("srec")
languages["Swift"] = require("swift")
languages["TCL"] = require("tcl")
languages["Tektronix extended HEX"] = require("tehex")
languages["TeX"] = require("tex")
languages["Text"] = require("text")
languages["VB / VBS"] = require("vb")
languages["txt2tags"] = require("txt2tags")
languages["Verilog"] = require("verilog")
languages["VHDL"] = require("vhdl")
languages["Visual Prolog"] = require("visualprolog")
languages["XML"] = require("xml")
languages["YAML"] = require("yaml")
