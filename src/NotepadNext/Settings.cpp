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


#include "Settings.h"


Settings::Settings(QObject *parent) : QObject(parent)
{
}

bool Settings::showMenuBar() const { return m_showMenuBar; }
bool Settings::showToolBar() const { return m_showToolBar; }
bool Settings::showTabBar() const { return m_showTabBar; }
bool Settings::showStatusBar() const { return m_showStatusBar; }

bool Settings::tabsClosable() const { return m_tabsClosable; }

bool Settings::restorePreviousSession() const { return m_restorePreviousSession; }
bool Settings::restoreTempFiles() const { return m_restoreTempFiles; }


void Settings::setShowMenuBar(bool showMenuBar)
{
    if (m_showMenuBar == showMenuBar)
        return;

    m_showMenuBar = showMenuBar;
    emit showMenuBarChanged(m_showMenuBar);
}

void Settings::setShowToolBar(bool showToolBar)
{
    if (m_showToolBar == showToolBar)
        return;

    m_showToolBar = showToolBar;
    emit showToolBarChanged(m_showToolBar);
}

void Settings::setShowTabBar(bool showTabBar)
{
    if (m_showTabBar == showTabBar)
        return;

    m_showTabBar = showTabBar;
    emit showTabBarChanged(m_showTabBar);
}

void Settings::setShowStatusBar(bool showStatusBar)
{
    if (m_showStatusBar == showStatusBar)
        return;

    m_showStatusBar = showStatusBar;
    emit showStatusBarChanged(m_showStatusBar);
}

void Settings::setTabsClosable(bool tabsClosable)
{
    if (m_tabsClosable == tabsClosable)
        return;

    m_tabsClosable = tabsClosable;
    emit tabsClosableChanged(m_tabsClosable);
}

void Settings::setRestorePreviousSession(bool restorePreviousSession)
{
    if (m_restorePreviousSession == restorePreviousSession)
        return;

    m_restorePreviousSession = restorePreviousSession;
    emit tabsClosableChanged(m_restorePreviousSession);
}

void Settings::setRestoreTempFiles(bool restoreTempFiles)
{
    if (m_restoreTempFiles == restoreTempFiles)
        return;

    m_restoreTempFiles = restoreTempFiles;
    emit tabsClosableChanged(m_restoreTempFiles);
}
