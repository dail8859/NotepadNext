local markdown = require("markdown")

local L = {}

for key, value in pairs(markdown) do
    L[key] = value
end

L.extensions = {
    "qd",
}

return L
