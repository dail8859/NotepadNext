function rgb(x)
    return ((x & 0xFF) << 16) | (x & 0xFF00) | ((x & 0xFF0000) >> 16)
end

languages = {}
languages["C++"] = require("cpp")
languages["Lua"] = require("lua")
languages["Text"] = require("text")
languages["Python"] = require("python")
