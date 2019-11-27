#include <sstream>
#include <string>

std::string dumpLuaState(lua_State *L) {
	std::stringstream ostr;
	int i;
	int top = lua_gettop(L);
	ostr << "top=" << top << ":\n";
	for (i = 1; i <= top; ++i) {
		int t = lua_type(L, i);
		switch(t) {
		case LUA_TSTRING:
			ostr << "  " << i << ": '" << lua_tostring(L, i) << "'\n";
			break;
		case LUA_TBOOLEAN:
			ostr << "  " << i << ": " << 
					(lua_toboolean(L, i) ? "true" : "false") << "\n";
			break;
		case LUA_TNUMBER:
			ostr << "  " << i << ": " << lua_tonumber(L, i) << "\n";
			break;
		default:
			ostr << "  " << i << ": TYPE=" << lua_typename(L, t) << "\n";
			break;
		}
	}
	return ostr.str();
}
