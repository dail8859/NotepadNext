function rgb(x)
    return ((x & 0xFF) << 16) | (x & 0xFF00) | ((x & 0xFF0000) >> 16)
end

languages = {}
languages["cpp"] = require("cpp")
languages["lua"] = require("lua")
languages["null"] = require("text")
languages["python"] = require("python")
