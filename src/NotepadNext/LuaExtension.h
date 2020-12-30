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


#ifndef LUAEXTENSION_H
#define LUAEXTENTION_H

#include "ScintillaNext.h"
#include <vector>

class ScintillaNext;
struct lua_State;

class LuaExtension final {
private:
    LuaExtension(); // Singleton
    LuaExtension(const LuaExtension &);   // Disable copy ctor
    void operator=(const LuaExtension &); // Disable operator=

public:
    static LuaExtension &Instance();

    ~LuaExtension();

    bool Initialise(lua_State *L, ScintillaNext *editor_);
    void setEditor(ScintillaEdit *editor_);
    bool Finalise();

    // Helper methods
    bool RunString(const char *s);
    bool OnExecute(const char *s);
    void CallShortcut(int id);

    // Scintilla callbacks
    //bool OnStyle(unsigned int startPos, int lengthDoc, int initStyle, StyleWriter *styler);
    //bool OnChar(const SCNotification *sc);
    //bool OnSavePointReached(const SCNotification *sc);
    //bool OnSavePointLeft(const SCNotification *sc);
    //bool OnModifyAttemptRO(const SCNotification *sc);
    //bool OnDoubleClick(const SCNotification *sc);
    //bool OnUpdateUI(const SCNotification *sc);
    //bool OnModification(const SCNotification *sc);
    //bool OnMacroRecord(const SCNotification *sc);
    //bool OnMarginClick(const SCNotification *sc);
    //bool OnNeedShown(const SCNotification *sc);
    //bool OnPainted(const SCNotification *sc);
    //bool OnUserListSelection(const SCNotification *sc);
    //bool OnDwellStart(const SCNotification *sc);
    //bool OnDwellEnd(const SCNotification *sc);
    //bool OnZoom(const SCNotification *sc);
    //bool OnHotSpotClick(const SCNotification *sc);
    //bool OnHotSpotDoubleClick(const SCNotification *sc);
    //bool OnHotSpotReleaseClick(const SCNotification *sc);
    //bool OnIndicatorClick(const SCNotification *sc);
    //bool OnIndicatorRelease(const SCNotification *sc);
    //bool OnCallTipClick(const SCNotification *sc);
    //bool OnAutoCSelection(const SCNotification *sc);
    //bool OnAutoCCancelled(const SCNotification *sc);
    //bool OnAutoCCharDeleted(const SCNotification *sc);
    //bool OnFocusIn(const SCNotification *sc);
    //bool OnFocusOut(const SCNotification *sc);

    //// Notepad++ callbacks
    //bool OnReady();
    //bool OnToolBarModification();
    //bool OnBeforeClose(const char *filename, uptr_t bufferid);
    //bool OnOpen(const char *filename, uptr_t bufferid);
    //bool OnClose();
    //bool OnBeforeOpen(const char *filename, uptr_t bufferid);
    //bool OnBeforeSave(const char *filename, uptr_t bufferid);
    //bool OnSave(const char *filename, uptr_t bufferid);
    //bool OnShutdown();
    //bool OnSwitchFile(const char *filename, uptr_t bufferid);
    //bool OnLangChange();
    //bool OnFileBeforeLoad();
    //bool OnFileLoadFailed();
    //bool OnReadOnlyChanged(const char *filename, uptr_t bufferid, int status);
    //bool OnDocOrderChanged(const char *filename, uptr_t bufferid, int newIndex);
    //bool OnSnapshotDirtyFileLoaded(const char *filename, uptr_t bufferid);
    //bool OnBeforeShutdown();
    //bool OnCancelShutdown();
    //bool OnFileBeforeRename(const char *filename, uptr_t bufferid);
    //bool OnFileRenameCancel(const char *filename, uptr_t bufferid);
    //bool OnFileRenamed(const char *filename, uptr_t bufferid);
    //bool OnFileBeforeDelete(const char *filename, uptr_t bufferid);
    //bool OnFileDeleteFailed(const char *filename, uptr_t bufferid);
    //bool OnFileDeleted();
};

#endif
