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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <string>

#include "Scintilla.h"
#include "LuaExtension.h"

#include "IFaceTableMixer.h"
#include "SciIFaceTable.h"

#include "lua.hpp"

IFaceTableMixer ifacemixer;

// From lua.c
#define EOFMARK         "<eof>"
#define marklen         (sizeof(EOFMARK)/sizeof(char) - 1)

static bool call_function(lua_State *L, int nargs, bool ignoreFunctionReturnValue);

// Helper function from SciTE
[[maybe_unused]] static int Substitute(std::string &s, const std::string &sFind, const std::string &sReplace) {
    int c = 0;
    size_t lenFind = sFind.size();
    size_t lenReplace = sReplace.size();
    size_t posFound = s.find(sFind);
    while (posFound != std::string::npos) {
        s.replace(posFound, lenFind, sReplace);
        posFound = s.find(sFind, posFound + lenReplace);
        c++;
    }
    return c;
}

// Helper function from SciTE
[[maybe_unused]] static bool Exists(const char *fileName) {
    bool ret = false;
    if (fileName && fileName[0]) {
        FILE *fp = fopen(fileName, "rb");
        if (fp) {
            ret = true;
            fclose(fp);
        }
    }
    return ret;
}


// A note on naming conventions:
// I've gone back and forth on this a bit, trying different styles.
// It isn't easy to get something that feels consistent, considering
// that the Lua API uses lower case, underscore-separated words and
// Scintilla of course uses mixed case with no underscores.

// What I've settled on is that functions that require you to think
// about the Lua stack are likely to be mixed with Lua API functions,
// so these should using a naming convention similar to Lua itself.
// Functions that don't manipulate Lua at a low level should follow
// the normal SciTE convention.  There is some grey area of course,
// and for these I just make a judgement call

static ScintillaEdit *editor;
static lua_State *luaState = 0;
static bool luaDisabled = false;
static bool tracebackEnabled = true;

typedef int NppExtensionAPIPane;
void hostTrace(const char *s)
{
    qInfo("%s", s);
}
void hostTraceError(const char *s)
{
    qWarning("%s", s);
}

// Forward declarations
static NppExtensionAPIPane check_pane_object(lua_State *L, int index);
static void push_pane_object(lua_State *L, NppExtensionAPIPane p);
static int iface_function_helper(lua_State *L, const IFaceFunction &func);

inline void raise_error(lua_State *L, const char *errMsg=NULL) {
    luaL_where(L, 1);
    if (errMsg) {
        lua_pushstring(L, errMsg);
    } else {
        lua_insert(L, -2);
    }
    lua_concat(L, 2);
    lua_error(L);
}

inline void raise_ferror(lua_State *L, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    lua_pushvfstring(L, fmt, args);
    va_end(args);
    raise_error(L);
}

inline int absolute_index(lua_State *L, int index) {
    return ((index < 0) && (index != LUA_REGISTRYINDEX)) ? (lua_gettop(L) + index + 1) : index;
}

[[maybe_unused]] static int cf_npp_send(lua_State *L) {
    // This is reinstated as a replacement for the old <pane>:send, which was removed
    // due to safety concerns.  Is now exposed as npp.SendEditor / npp.SendOutput.
    // It is rewritten to be typesafe, checking the arguments against the metadata in
    // IFaceTable in the same way that the object interface does.

    IFaceFunction propfunc;
    int paneIndex = lua_upvalueindex(1);
    check_pane_object(L, paneIndex);
    int message = (int)luaL_checkinteger(L, 1);

    lua_pushvalue(L, paneIndex);
    lua_replace(L, 1);

    const IFaceFunction *func;
    func = SciIFaceTable.GetFunctionByMessage(message);

    if (func == nullptr) {
        propfunc = SciIFaceTable.GetPropertyFuncByMessage(message);
        if (propfunc.value != -1) func = &propfunc;
    }

    if (func != nullptr) {
        if (IFaceFunctionIsScriptable(*func)) {
            return iface_function_helper(L, *func);
        } else {
            raise_error(L, "Cannot call send for this function: not scriptable.");
            return 0;
        }
    } else {
        raise_error(L, "Message number does not match any published Scintilla / Notepad++ function or property");
        return 0;
    }
}

[[maybe_unused]] static int cf_npp_constname(lua_State *L) {
    char constName[100] = "";
    const char *hint = nullptr;
    int message = (int)luaL_checkinteger(L, 1);

    hint = luaL_optstring(L, 2, nullptr);

    if (ifacemixer.GetConstantName(message, constName, 100, hint) > 0) {
        lua_pushstring(L, constName);
        return 1;
    } else {
        raise_error(L, "Argument does not match any Scintilla / Notepad++ constant");
        return 0;
    }
}

void stackdump(lua_State* l)
{
    int i;
    int top = lua_gettop(l);

    for (i = 1; i <= top; i++)
    {  /* repeat for each level */
        int t = lua_type(l, i);
        hostTrace(lua_typename(l, t));
        hostTrace("  ");  /* put a separator */
    }
    hostTrace("\n");  /* end the listing */
}

static NppExtensionAPIPane check_pane_object(lua_State *L, int index) {
    NppExtensionAPIPane *pPane = static_cast<NppExtensionAPIPane *>(luaL_testudata(L, index, "Nn_MT_Pane"));

    if ((!pPane) && lua_istable(L, index)) {
        // so that nested objects have a convenient way to do a back reference
        int absIndex = absolute_index(L, index);
        lua_pushliteral(L, "pane");
        lua_gettable(L, absIndex);
        pPane = static_cast<NppExtensionAPIPane *>(luaL_testudata(L, -1, "Nn_MT_Pane"));
    }

    if (pPane) {
        return *pPane;
    }

    pPane = static_cast<NppExtensionAPIPane *>(luaL_testudata(L, index, "Nn_MT_Application"));

    // NOTE: I'm not sure what the above comment about the "back reference" means. This may or
    // may not apply in this case. So that if statement may need pasted/modified in this case

    if (pPane) {
        return *pPane;
    }

    if (index == 1)
        lua_pushliteral(L, "Self object is missing in pane method or property access.");
    else if (index == lua_upvalueindex(1))
        lua_pushliteral(L, "Internal error: pane object expected in closure.");
    else
        lua_pushliteral(L, "Pane object expected.");

    raise_error(L);
    return 0; // this line never reached
}

static int cf_global_print(lua_State *L) {
    int nargs = lua_gettop(L);

    lua_getglobal(L, "tostring");

    for (int i = 1; i <= nargs; ++i) {
        if (i > 1)
            hostTrace("\t");

        const char *argStr = lua_tostring(L, i);
        if (argStr) {
            hostTrace(argStr);
        } else {
            lua_pushvalue(L, -1); // tostring
            lua_pushvalue(L, i);
            lua_call(L, 1, 1);
            argStr = lua_tostring(L, -1);
            if (argStr) {
                hostTrace(argStr);
            } else {
                raise_error(L, "tostring (called from print) returned a non-string");
            }
            lua_settop(L, nargs + 1);
        }
    }

    hostTrace("\n");
    return 0;
}

[[maybe_unused]] static bool call_function(lua_State *L, int nargs, bool ignoreFunctionReturnValue=false) {
    bool handled = false;
    if (L) {
        int traceback = 0;
        if (tracebackEnabled) {
            lua_getglobal(L, "debug");
            lua_getfield(L, -1, "traceback");
            lua_remove(L, -2);
            if (lua_isfunction(L, -1)) {
                traceback = lua_gettop(L) - nargs - 1;
                lua_insert(L, traceback);
            } else {
                lua_pop(L, 1);
            }
        }

        int result = lua_pcall(L, nargs, ignoreFunctionReturnValue ? 0 : 1, traceback);

        if (traceback) {
            lua_remove(L, traceback);
        }

        if (result == LUA_OK) {
            if (ignoreFunctionReturnValue) {
                handled = true;
            } else {
                handled = (0 != lua_toboolean(L, -1));
                lua_pop(L, 1);
            }
        } else if (result == LUA_ERRRUN) {
            lua_getglobal(L, "print");
            lua_insert(L, -2); // use pushed error message
            lua_pcall(L, 1, 0, 0);
        } else {
            lua_pop(L, 1);
            if (result == LUA_ERRMEM) {
                hostTraceError("Memory allocation error\n");
            } else if (result == LUA_ERRERR) {
                hostTraceError("An error occurred, but cannot be reported due to failure in _TRACEBACK\n");
            } else {
                hostTraceError("Unexpected error\n");
            }
        }
    }
    return handled;
}

struct ScintillaFailure {
    sptr_t status;
    explicit ScintillaFailure(sptr_t status_) : status(status_) {
    }
};

static int iface_function_helper(lua_State *L, const IFaceFunction &func) {
    NppExtensionAPIPane p = check_pane_object(L, 1);
    Q_UNUSED(p)

    int arg = 2;

    sptr_t params[2] = {0,0};

    char *stringResult = 0;
    enum stringResultType { none, string, tstring } needStringResult = none;
    int loopParamCount = 2;

    if (func.paramType[0] == iface_length && func.paramType[1] == iface_string) {
        params[0] = static_cast<sptr_t>(lua_rawlen(L, arg));
        params[1] = reinterpret_cast<sptr_t>(params[0] ? lua_tostring(L, arg) : "");
        loopParamCount = 0;
    } else if ((func.paramType[1] == iface_stringresult) || (func.returnType == iface_stringresult)) {
        needStringResult = (func.paramType[1] == iface_stringresult) || (func.returnType == iface_stringresult) ? string : tstring;
        // The buffer will be allocated later, so it won't leak if Lua does
        // a longjmp in response to a bad arg.
        if (func.paramType[0] == iface_length) {
            loopParamCount = 0;
        } else {
            loopParamCount = 1;
        }
    }

    // Specifically handle these 2 messages because they carry the setter in the 2nd param
    if (func.value == SCI_SETMARGINLEFT || func.value == SCI_SETMARGINRIGHT) {
        params[1] = static_cast<sptr_t>(luaL_checkinteger(L, arg++));
    }
    else {
        for (int i = 0; i < loopParamCount; ++i) {
            if (func.paramType[i] == iface_string) {
                const char *s = lua_tostring(L, arg++);
                params[i] = reinterpret_cast<sptr_t>(s ? s : "");
            } else if (func.paramType[i] == iface_keymod) {
                int keycode = static_cast<int>(luaL_checkinteger(L, arg++)) & 0xFFFF;
                int modifiers = static_cast<int>(luaL_checkinteger(L, arg++)) & (SCMOD_SHIFT|SCMOD_CTRL|SCMOD_ALT);
                params[i] = keycode | (modifiers<<16);
            } else if (func.paramType[i] == iface_bool) {
                params[i] = lua_toboolean(L, arg++);
            } else if (IFaceTypeIsNumeric(func.paramType[i])) {
                params[i] = static_cast<sptr_t>(luaL_checkinteger(L, arg++));
            }
        }
    }

    if (needStringResult != none) {
        sptr_t stringResultLen;
        if (needStringResult == string) {
            stringResultLen = editor->send(func.value, params[0], 0);
            if (stringResultLen > 0) {
                // not all string result methods are guaranteed to add a null terminator
                stringResult = new char[stringResultLen+1];
                stringResult[stringResultLen]='\0';
                params[1] = reinterpret_cast<sptr_t>(stringResult);
            } else {
                // Is this an error?  Are there any cases where it's not an error,
                // and where the right thing to do is just return a blank string?
                return 0;
            }
        }

        if (func.paramType[0] == iface_length) {
            params[0] = stringResultLen;
        }
    }

    // Now figure out what to do with the param types and return type.
    // - stringresult gets inserted at the start of return tuple.
    // - numeric return type gets returned to lua as a number (following the stringresult)
    // - other return types e.g. void get dropped.

    sptr_t result = 0;
    try {
        result = editor->send(func.value, params[0], params[1]);
    }
    catch (ScintillaFailure &sf) {
        std::string failureExplanation;
        failureExplanation += "Lua: Scintilla failure ";
        failureExplanation += std::to_string(sf.status);
        failureExplanation += " for message ";
        failureExplanation += std::to_string(func.value);
        failureExplanation += ".\n";
        // Reset status before continuing
        editor->send(SCI_SETSTATUS, SC_STATUS_OK, 0);
        hostTraceError(failureExplanation.c_str());
    }

    int resultCount = 0;

    if (stringResult) {
        lua_pushstring(L, stringResult);
        delete[] stringResult;
        resultCount++;
    }

    if (func.returnType == iface_bool) {
        lua_pushboolean(L, static_cast<int>(result));
        resultCount++;
    } else if (IFaceTypeIsNumeric(func.returnType)) {
        lua_pushinteger(L, static_cast<lua_Integer>(result));
        resultCount++;
    }

    return resultCount;
}

struct IFacePropertyBinding {
    NppExtensionAPIPane pane;
    const IFaceProperty *prop;
};

static int cf_ifaceprop_metatable_index(lua_State *L) {
    // if there is a getter, __index calls it
    // otherwise, __index raises "property 'name' is write-only".
    IFacePropertyBinding *ipb = static_cast<IFacePropertyBinding *>(luaL_testudata(L, 1, "Nn_MT_IFacePropertyBinding"));
    if (!(ipb && IFacePropertyIsScriptable(*(ipb->prop)))) {
        raise_error(L, "Internal error: property binding is improperly set up");
        return 0;
    }
    if (ipb->prop->getter == 0) {
        raise_error(L, "Attempt to read a write-only indexed property");
        return 0;
    }
    IFaceFunction func = ipb->prop->GetterFunction();

    // rewrite the stack to match what the function expects.  put pane at index 1; param is already at index 2.
    push_pane_object(L, ipb->pane);
    lua_replace(L, 1);
    lua_settop(L, 2);
    return iface_function_helper(L, func);
}

static int cf_ifaceprop_metatable_newindex(lua_State *L) {
    IFacePropertyBinding *ipb = static_cast<IFacePropertyBinding *>(luaL_testudata(L, 1, "Nn_MT_IFacePropertyBinding"));
    if (!(ipb && IFacePropertyIsScriptable(*(ipb->prop)))) {
        raise_error(L, "Internal error: property binding is improperly set up");
        return 0;
    }
    if (ipb->prop->setter == 0) {
        raise_error(L, "Attempt to write a read-only indexed property");
        return 0;
    }
    IFaceFunction func = ipb->prop->SetterFunction();

    // rewrite the stack to match what the function expects.
    // pane at index 1; param at index 2, value at index 3
    push_pane_object(L, ipb->pane);
    lua_replace(L, 1);
    lua_settop(L, 3);
    return iface_function_helper(L, func);
}

static int cf_pane_iface_function(lua_State *L) {
    int funcidx = lua_upvalueindex(1);
    const IFaceFunction *func = static_cast<IFaceFunction *>(lua_touserdata(L, funcidx));
    if (func) {
        return iface_function_helper(L, *func);
    } else {
        raise_error(L, "Internal error - bad upvalue in iface function closure");
        return 0;
    }
}

static int push_iface_function(lua_State *L, const char *name, IFaceTableInterface *iface) {
    auto func = iface->FindFunction(name);
    if (func != nullptr) {
        if (IFaceFunctionIsScriptable(*func)) {
            lua_pushlightuserdata(L, (void*)func);
            lua_pushcclosure(L, cf_pane_iface_function, 1);

            // Since Lua experts say it is inefficient to create closures / cfunctions
            // in an inner loop, I tried caching the closures in the metatable, and looking
            // for them there first.  However, it made very little difference and did not
            // seem worth the added complexity. - WBD

            return 1;
        }
    }
    return -1; // signal to try next pane index handler
}

static int push_iface_propval(lua_State *L, const char *name, IFaceTableInterface *iface) {
    // this function doesn't raise errors, but returns 0 if the function is not handled.

    auto prop = iface->FindProperty(name);
    if (prop != nullptr) {
        if (!IFacePropertyIsScriptable(*prop)) {
            raise_error(L, "Error: iface property is not scriptable.");
            return -1;
        }

        if (prop->paramType == iface_void) {
            if (prop->getter) {
                lua_settop(L, 1);
                return iface_function_helper(L, prop->GetterFunction());
            }
        } else if (prop->paramType == iface_bool) {
            // The bool getter is untested since there are none in the iface.
            // However, the following is suggested as a reference protocol.
            NppExtensionAPIPane p = check_pane_object(L, 1);
            Q_UNUSED(p)

            if (prop->getter) {
                if (editor->send(prop->getter, 1, 0)) {
                    lua_pushnil(L);
                    return 1;
                } else {
                    lua_settop(L, 1);
                    lua_pushboolean(L, 0);
                    return iface_function_helper(L, prop->GetterFunction());
                }
            }
        } else {
            // Indexed property.  These return an object with the following behavior:
            // if there is a getter, __index calls it
            // otherwise, __index raises "property 'name' is write-only".
            // if there is a setter, __newindex calls it
            // otherwise, __newindex raises "property 'name' is read-only"

            IFacePropertyBinding *ipb = static_cast<IFacePropertyBinding *>(lua_newuserdata(L, sizeof(IFacePropertyBinding)));
            if (ipb) {
                ipb->pane = check_pane_object(L, 1);
                ipb->prop = prop;
                if (luaL_newmetatable(L, "Nn_MT_IFacePropertyBinding")) {
                    lua_pushliteral(L, "__index");
                    lua_pushcfunction(L, cf_ifaceprop_metatable_index);
                    lua_settable(L, -3);
                    lua_pushliteral(L, "__newindex");
                    lua_pushcfunction(L, cf_ifaceprop_metatable_newindex);
                    lua_settable(L, -3);
                }
                lua_setmetatable(L, -2);
                return 1;
            } else {
                raise_error(L, "Internal error: failed to allocate userdata for indexed property");
                return -1;
            }
        }
    }

    return -1; // signal to try next pane index handler
}

static int cf_pane_metatable_index(lua_State *L) {
    IFaceTableInterface *iface = static_cast<IFaceTableInterface *>(lua_touserdata(L, lua_upvalueindex(1)));
    if (lua_isstring(L, 2)) {
        const char *name = lua_tostring(L, 2);

        // these return the number of values pushed (possibly 0), or -1 if no match
        int results = push_iface_function(L, name, iface);
        if (results < 0)
            results = push_iface_propval(L, name, iface);

        if (results >= 0) {
            return results;
        } else if (name[0] != '_') {
            lua_getmetatable(L, 1);
            if (lua_istable(L, -1)) {
                lua_pushvalue(L, 2);
                lua_gettable(L, -2);
                if (!lua_isnil(L, -1))
                    return 1;
            }
        }
    }

    raise_error(L, "Pane function / readable property / indexed writable property name expected");
    return 0;
}

static int cf_pane_metatable_newindex(lua_State *L) {
    IFaceTableInterface *iface = static_cast<IFaceTableInterface *>(lua_touserdata(L, lua_upvalueindex(1)));
    if (lua_isstring(L, 2)) {
        auto prop = iface->FindProperty(lua_tostring(L, 2));
        if (prop != nullptr) {
            if (IFacePropertyIsScriptable(*prop)) {
                if (prop->setter) {
                    // stack needs to be rearranged to look like an iface function call
                    lua_remove(L, 2);
                    if (prop->paramType == iface_void) {
                        return iface_function_helper(L, prop->SetterFunction());
                    } else if ((prop->paramType == iface_bool)) {
                        if (!lua_isnil(L, 3)) {
                            lua_pushboolean(L, 1);
                            lua_insert(L, 2);
                        } else {
                            // the nil will do as a false value.
                            // just push an arbitrary numeric value that Scintilla will ignore
                            lua_pushinteger(L, 0);
                        }
                        return iface_function_helper(L, prop->SetterFunction());

                    } else {
                        raise_error(L, "Error - (pane object) cannot assign directly to indexed property");
                    }
                } else {
                    raise_error(L, "Error - (pane object) cannot assign to a read-only property");
                }
            }
        }
    }

    raise_error(L, "Error - (pane object) expected the name of a writable property");
    return 0;
}

void push_pane_object(lua_State *L, NppExtensionAPIPane p) {
    *static_cast<NppExtensionAPIPane *>(lua_newuserdata(L, sizeof(p))) = p;
    if (luaL_newmetatable(L, "Nn_MT_Pane")) {
        lua_pushlightuserdata(L, &SciIFaceTable);
        lua_pushcclosure(L, cf_pane_metatable_index, 1);
        lua_setfield(L, -2, "__index");
        lua_pushlightuserdata(L, &SciIFaceTable);
        lua_pushcclosure(L, cf_pane_metatable_newindex, 1);
        lua_setfield(L, -2, "__newindex");

        // Push built-in functions into the metatable, where the custom
        // __index metamethod will find them.
    }
    lua_setmetatable(L, -2);
}

static int cf_global_metatable_index(lua_State *L) {
    if (lua_isstring(L, 2)) {
        const char *name = lua_tostring(L, 2);
        if ((name[0] < 'A') || (name[0] > 'Z') || ((name[1] >= 'a') && (name[1] <= 'z'))) {
            // short circuit; iface constants are always upper-case and start with a letter
            return 0;
        }

        auto con = ifacemixer.FindConstant(name);
        if (con != nullptr) {
            lua_pushinteger(L, con->value);
            return 1;
        } else {
            auto func = ifacemixer.FindFunctionByConstantName(name);
            if (func != nullptr) {
                lua_pushinteger(L, func->value);

                // FindFunctionByConstantName is slow, so cache the result into the
                // global table.  My tests show this gives an order of magnitude
                // improvement.
                lua_pushvalue(L, 2);
                lua_pushvalue(L, -2);
                lua_rawset(L, 1);

                return 1;
            }
        }
    }

    return 0; // global namespace access should not raise errors
}

static int LuaPanicFunction(lua_State *L) {
    if (L == luaState) {
        lua_close(luaState);
        luaState = NULL;
        luaDisabled = true;
    }
    hostTraceError("\nError occurred in unprotected call.  This is very bad.\n");
    return 1;
}

static bool InitGlobalScope() {
    tracebackEnabled = true;

    if (!luaDisabled) {
        if (!luaState) {
            luaDisabled = true;
            hostTraceError("Scripting engine failed to initialise\n");
            return false;
        }
        lua_atpanic(luaState, LuaPanicFunction);

    }
    else {
        return false;
    }

    ifacemixer.AddIFaceTable(&SciIFaceTable);

    // ...register standard libraries
    luaL_openlibs(luaState);

    // override a library function whose default impl uses stdout
    lua_register(luaState, "print", cf_global_print);

    // pane objects
    push_pane_object(luaState, 0);
    lua_setglobal(luaState, "editor");

    // get global environment table from registry
    lua_pushglobaltable(luaState);
    // Metatable for global namespace, to publish iface constants
    if (luaL_newmetatable(luaState, "Nn_MT_GlobalScope")) {
        lua_pushcfunction(luaState, cf_global_metatable_index);
        lua_setfield(luaState, -2, "__index");
    }
    // set global index callback hook
    lua_setmetatable(luaState, -2);
    // remove the global environment table from the stack
    lua_pop(luaState, 1);

    return true;
}


LuaExtension::LuaExtension() {}
LuaExtension::~LuaExtension() {}

LuaExtension &LuaExtension::Instance() {
    static LuaExtension singleton;
    return singleton;
}

bool LuaExtension::Initialise(lua_State *L, ScintillaNext *editor_) {
    luaState = L;
    editor = editor_;
    InitGlobalScope();

    return false;
}

void LuaExtension::setEditor(ScintillaEdit *editor_) {
    editor = editor_;
}

bool LuaExtension::Finalise() {
    if (luaState) {
        lua_close(luaState);
    }

    luaState = NULL;
    editor = NULL;

    return false;
}

bool LuaExtension::RunString(const char *s) {
    if (luaState || InitGlobalScope()) {
        int status = luaL_loadbuffer(luaState, s, strlen(s), "=File");

        if (status == LUA_OK) {
            status = lua_pcall(luaState, 0, LUA_MULTRET, 0);
        }

        if (status != LUA_OK) {
            // Print an error message
            hostTraceError(lua_tostring(luaState, -1));
            hostTraceError("\n");
            lua_settop(luaState, 0); /* clear stack */
            return false;
        }

        lua_settop(luaState, 0); /* clear stack */
    }

    return true;
}

bool LuaExtension::OnExecute(const char *s) {
    static bool isFirstLine = true;
    static std::string chunk;
    int status = 0;

    if (luaState || InitGlobalScope()) {
        if (isFirstLine) {
            // First try to compile the chunk as a return statement
            const char *retline = lua_pushfstring(luaState, "return %s;", s);
            status = luaL_loadbuffer(luaState, retline, strlen(retline), "=Console");
            if (status == 0) lua_remove(luaState, -2);
            else lua_pop(luaState, 2);

            if (status == LUA_OK) {
                // It worked, let's call it
                status = lua_pcall(luaState, 0, LUA_MULTRET, 0);
            }
            else {
                // Else let's just try it as is
                status = luaL_loadbuffer(luaState, s, strlen(s), "=Console");
                if (status == LUA_OK) {
                    status = lua_pcall(luaState, 0, LUA_MULTRET, 0);
                }
                else if (status == LUA_ERRSYNTAX) {
                    size_t lmsg;
                    const char *msg = lua_tolstring(luaState, -1, &lmsg);
                    if (lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
                        lua_pop(luaState, 1);
                        isFirstLine = false;
                        chunk = s;
                        return false;
                    }
                }
            }
        }
        else {
            // Append the new line to what we've gotten so far
            chunk.append("\n");
            chunk.append(s);
            status = luaL_loadbuffer(luaState, chunk.c_str(), chunk.length(), "=Console");
            if (status == LUA_OK) {
                status = lua_pcall(luaState, 0, LUA_MULTRET, 0);
            }
            else if (status == LUA_ERRSYNTAX) {
                size_t lmsg;
                const char *msg = lua_tolstring(luaState, -1, &lmsg);
                if (lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
                    lua_pop(luaState, 1);
                    return false;
                }
            }
        }

        // At this point *something* ran so clear out some data
        chunk.clear();
        isFirstLine = true;

        if (status == LUA_OK) {
            if (lua_gettop(luaState) > 0) {  /* any result to print? */
                lua_getglobal(luaState, "print");
                lua_insert(luaState, 1);
                if (lua_pcall(luaState, lua_gettop(luaState) - 1, 0, 0) != 0)
                    hostTraceError("error calling " LUA_QL("print"));
            }
            // else everything finished fine but had no return value
        }
        else {
            // Print an error message if possible
            const char *errmsg = lua_tostring(luaState, -1);
            if (errmsg) {
                hostTraceError(errmsg);
                hostTraceError("\n");
            }
            else {
                hostTraceError("error\n");
            }
        }
        lua_settop(luaState, 0); /* clear stack */
    }
    return true;
}
