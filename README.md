# Notepad Next

![Build Notepad Next](https://github.com/dail8859/NotepadNext/workflows/Build%20Notepad%20Next/badge.svg)

A cross-platform, reimplementation of Notepad++. Releases are available for Windows, Linux, and macOS.

Though the application overall is stable and usable, it should not be considered safe for critically important work.

There are numerous bugs and half working implementations. Pull requests are greatly appreciated.

![screenshot](/doc/screenshot.png)

# Development
Current development is done using Visual Studio 2019 and Qt v5.15 on Windows. This is also known to build successfully on various Linux distributions and macOS. Other platforms/compilers should be usable with minor modifications.

If you are familiar with building C++ Qt desktop applications with Qt Creator, then this should be as simple as opening `src/NotepadNext.pro` and build/run the project.

If you are new to building C++ Qt desktop applications, there is a more detailed guide [here](/doc/Building.md).


# License
This code is released under the [GNU General Public License version 3](http://www.gnu.org/licenses/gpl-3.0.txt).
