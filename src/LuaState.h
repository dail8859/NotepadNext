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


#ifndef LUASTATE_H
#define LUASTATE_H

#include <QString>
#include <QStringList>
#include <QtGlobal>
#include "lua.hpp"

struct lua_State;

class LuaState
{
public:
    LuaState();
    ~LuaState();

    void execute(const char *statement, bool clear = true);
    void executeFile(const QString &fileName);

    template<typename T>
    T executeAndReturn(const char *statement);

    void clearStack();

    void raiseError(const char *errorMessage = Q_NULLPTR);

    lua_State *L = Q_NULLPTR;

private:
};

template<>
inline bool LuaState::executeAndReturn(const char *statement)
{
    execute(statement, false);
    luaL_checktype(L, 1, LUA_TBOOLEAN);

    bool b = lua_toboolean(L, 1);
    lua_pop(L, 1);

    return b;
}

template<>
inline QString LuaState::executeAndReturn(const char *statement)
{
    execute(statement, false);
    luaL_checktype(L, 1, LUA_TSTRING);

    QString s(lua_tostring(L, 1));
    lua_pop(L, 1);

    return s;
}

template<>
inline QStringList LuaState::executeAndReturn(const char *statement)
{
    execute(statement, false);
    luaL_checktype(L, 1, LUA_TTABLE);

    QStringList list;

    /* table is in the stack at index 't' */
    lua_pushnil(L);  /* first key */
    while (lua_next(L, 1) != 0) {
        list << lua_tostring(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return list;
}


#endif // LUASTATE_H
