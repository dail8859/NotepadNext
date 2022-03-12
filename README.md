# Notepad Next

| Branch | Status |
|---|---|
| master | ![Build Notepad Next](https://github.com/dail8859/NotepadNext/workflows/Build%20Notepad%20Next/badge.svg) |
| dev | ![Build Notepad Next](https://github.com/dail8859/NotepadNext/workflows/Build%20Notepad%20Next/badge.svg?branch=dev) |



A cross-platform, reimplementation of Notepad++. Releases are available for Windows and Linux.

Though the application overall is stable and usable, it should not be considered safe for critically important work.

There are numerous bugs and half working implementations. Pull requests are greatly appreciated.

![screenshot](/doc/screenshot.png)

# Development
Current development is done using Visual Studio 2019 and Qt v5.15 on Windows. This is known to build successfully on Ubuntu 21.10. Other Linux distributions are likely to work as well. Other platforms/compilers have not been tested but should be usable with minor modifications.

If you are familiar with building C++ Qt desktop applications with Qt Creator, then this should be as simple as opening `src/NotepadNext.pro` and build/run the project.

If you are new to building C++ Qt desktop applications, there is a more detailed guide [here](/doc/Building.md).


# License
This code is released under the [GNU General Public License version 3](http://www.gnu.org/licenses/gpl-3.0.txt).
