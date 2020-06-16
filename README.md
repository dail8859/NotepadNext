# Notepad Next

[![Build status](https://ci.appveyor.com/api/projects/status/ix8wdx6tu5t3you0?svg=true)](https://ci.appveyor.com/project/dail8859/notepadnext)

A cross-platform, reimplementation of Notepad++.

Though the application overall is stable and usable, it should not be considered safe for critically important work.

There are numerous bugs and half working implementations. Pull requests are greatly appreciated.

![screenshot](/doc/screenshot.png)

# Development
Current development has been done using Visual Studio 2019 and Qt v5.15. Other platforms/compilers have not been tested but should be usable.

1. Install Visual Studio 2019 (Community Edition is fine)
1. Download the [Qt Installer](https://www.qt.io/download-qt-installer)
1. Use the Qt Installer to install `Qt 5.15.X` (MSVC 2019 64-bit) and `Qt Creator` 
1. Open `src/NotepadNext.pro`
1. Build/Run the project

# License
This code is released under the [GNU General Public License version 3](http://www.gnu.org/licenses/gpl-3.0.txt).
