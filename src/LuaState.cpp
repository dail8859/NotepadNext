/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "LuaState.h"
#include "lua.hpp"

#include <QFile>

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

static int require_resource(lua_State *L)
{
    const char *module = luaL_checkstring(L, 1);

    QString module_file = QString(":/languages/%1.lua").arg(module);
    QFile f(module_file);

    lua_pop(L, 1);

    if (f.exists()) {
        f.open(QFile::ReadOnly);
        luaL_loadstring(L, f.readAll().constData());
        f.close();

        // At this point either the function or an error string is on the stack
    }
    else {
        lua_pushfstring(L, "\n\tno file '%s'", module_file.toLatin1().constData());
    }

    return 1;
}

LuaState::LuaState()
{
    qInfo(Q_FUNC_INFO);

    L = luaL_newstate();
    luaL_openlibs(L);

    // Remove the searchers used for finding files on the file system
    execute("table.remove(package.searchers, 4); table.remove(package.searchers, 3); table.remove(package.searchers, 2)");

    for(int i = 0; i < 3; ++i) {
        lua_getglobal(L, "table");
        lua_getfield(L, -1, "remove");
        lua_remove(L, -2);

        lua_getglobal(L, "package");
        lua_getfield(L, -1, "searchers");
        lua_remove(L, -2);
        lua_call(L, 1, 0);
    }

    lua_getglobal(L, "table");
    lua_getfield(L, -1, "insert");
    lua_remove(L, -2);

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "searchers");
    lua_remove(L, -2);

    lua_pushcfunction(L, require_resource);

    lua_call(L, 2, 0);
}

LuaState::~LuaState()
{
    if (L) {
        lua_close(L);
        L = Q_NULLPTR;
    }
}

void LuaState::execute(const char *statement, bool clear)
{
    // There may be other things on the stack so save the top of it
    const int stacktop = lua_gettop(L);

    int status = luaL_loadstring(L, statement);

    if (status == LUA_OK) {
        status = lua_pcall(L, 0, LUA_MULTRET, 0);
    }
    else if (status == LUA_ERRSYNTAX) {
        qWarning("LUA_ERRSYNTAX: %s", statement);
    }
    else if (status == LUA_ERRMEM) {
        qFatal("Lua memory allocation error");
    }

    if (status != LUA_OK) {
        // Print an error message
        //writeErrorToOutput(lua_tostring(L, -1));
        //writeErrorToOutput("\r\n");
        qWarning("%s", lua_tostring(L, -1));
    }

    if (clear)
        lua_settop(L, stacktop);
}

void LuaState::executeFile(const QString &fileName)
{
    QFile ff(fileName);

    if (!ff.open(QFile::ReadOnly)) {
        qFatal("Cannot execute file: %s", fileName.toLatin1().constData());
    }

    execute(ff.readAll().constData());

    ff.close();
}

void LuaState::clearStack()
{
    lua_settop(L, 0);
}

void LuaState::raiseError(const char *errorMessage)
{
    luaL_where(L, 1);
    if (errorMessage) {
        lua_pushstring(L, errorMessage);
    } else {
        lua_insert(L, -2);
    }

    lua_concat(L, 2);
    lua_error(L);
}
