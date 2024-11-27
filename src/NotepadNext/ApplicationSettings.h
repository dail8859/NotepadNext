/*
 * This file is part of Notepad Next.
 * Copyright 2024 Justin Dailey
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


#pragma once

#include <functional>

#include <QObject>
#include <QSettings>
#include <QString>


template<typename T>
class ApplicationSetting
{
public:
    ApplicationSetting(const char * const key, T defaultValue = T())
        : mKey(key)
        , mDefault(defaultValue)
        , mCallable(Q_NULLPTR)
    {}

    ApplicationSetting(const char * const key, std::function<T()> callable)
        : mKey(key)
        , mDefault(T())
        , mCallable(callable)
    {}

    inline T getDefault() const { return mCallable ? mCallable() : mDefault ; }
    inline const char * const key() const { return mKey; }

private:
    const char * const mKey;
    const T mDefault;
    std::function<T()> mCallable;
};


#define DEFINE_SETTING(name, lname, type)\
public:\
    type lname() const;\
public slots:\
    void set##name(type lname);\
Q_SIGNAL\
    void lname##Changed(type lname);\


class ApplicationSettings : public QSettings
{
    Q_OBJECT

public:
    explicit ApplicationSettings(QObject *parent = nullptr);

public:
    template <typename T>
    T get(const char *key, const T &defaultValue) const
    { return value(QLatin1String(key), defaultValue).template value<T>(); }

    template <typename T>
    T get(const ApplicationSetting<T> &setting) const
    { return get(setting.key(), setting.getDefault()); }

    template <typename T>
    void set(const ApplicationSetting<T> &setting, const T &value)
    { setValue(QLatin1String(setting.key()), value); }

public:
    DEFINE_SETTING(ShowMenuBar, showMenuBar, bool)
    DEFINE_SETTING(ShowToolBar, showToolBar, bool)
    DEFINE_SETTING(ShowTabBar, showTabBar, bool)
    DEFINE_SETTING(ShowStatusBar, showStatusBar, bool)
    DEFINE_SETTING(CenterSearchDialog, centerSearchDialog, bool)

    DEFINE_SETTING(TabsClosable, tabsClosable, bool)
    DEFINE_SETTING(ExitOnLastTabClosed, exitOnLastTabClosed, bool)

    DEFINE_SETTING(CombineSearchResults, combineSearchResults, bool)

    DEFINE_SETTING(RestorePreviousSession, restorePreviousSession, bool)
    DEFINE_SETTING(RestoreUnsavedFiles, restoreUnsavedFiles, bool)
    DEFINE_SETTING(RestoreTempFiles, restoreTempFiles, bool)

    DEFINE_SETTING(Translation, translation, QString)

    DEFINE_SETTING(ShowWhitespace, showWhitespace, bool);
    DEFINE_SETTING(ShowEndOfLine, showEndOfLine, bool);
    DEFINE_SETTING(ShowWrapSymbol, showWrapSymbol, bool)
    DEFINE_SETTING(ShowIndentGuide, showIndentGuide, bool)
    DEFINE_SETTING(WordWrap, wordWrap, bool);
    DEFINE_SETTING(FontName, fontName, QString);
    DEFINE_SETTING(FontSize, fontSize, int);
    DEFINE_SETTING(AdditionalWordChars, additionalWordChars, QString);
};
