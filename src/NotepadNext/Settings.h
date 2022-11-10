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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool showMenuBar READ showMenuBar WRITE setShowMenuBar NOTIFY showMenuBarChanged)
    Q_PROPERTY(bool showToolBar READ showToolBar WRITE setShowToolBar NOTIFY showToolBarChanged)
    Q_PROPERTY(bool showTabBar READ showTabBar WRITE setShowTabBar NOTIFY showTabBarChanged)
    Q_PROPERTY(bool showStatusBar READ showStatusBar WRITE setShowStatusBar NOTIFY showStatusBarChanged)

    Q_PROPERTY(bool tabsClosable READ tabsClosable WRITE setTabsClosable NOTIFY tabsClosableChanged)

    Q_PROPERTY(bool restorePreviousSession READ restorePreviousSession WRITE setRestorePreviousSession NOTIFY restorePreviousSessionChanged)

    bool m_showMenuBar = true;
    bool m_showToolBar = true;
    bool m_showTabBar = true;
    bool m_showStatusBar = true;

    bool m_tabsClosable = true;

    bool m_restorePreviousSession = false;

public:
    explicit Settings(QObject *parent = nullptr);

    bool showMenuBar() const;
    bool showToolBar() const;
    bool showTabBar() const;
    bool showStatusBar() const;

    bool tabsClosable() const;

    bool restorePreviousSession() const;

signals:
    void showMenuBarChanged(bool showMenuBar);
    void showToolBarChanged(bool showToolBar);
    void showTabBarChanged(bool showTabBar);
    void showStatusBarChanged(bool showTabBar);

    void tabsClosableChanged(bool tabsClosable);

    void restorePreviousSessionChanged(bool restorePreviousSession);

public slots:
    void setShowMenuBar(bool showMenuBar);
    void setShowToolBar(bool showToolBar);
    void setShowTabBar(bool showTabBar);
    void setShowStatusBar(bool showStatusBar);

    void setTabsClosable(bool tabsClosable);

    void setRestorePreviousSession(bool restorePreviousSession);
};

#endif // SETTINGS_H
