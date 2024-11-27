# Notepad Next

![Build Notepad Next](https://github.com/dail8859/NotepadNext/workflows/Build%20Notepad%20Next/badge.svg)

A cross-platform, reimplementation of Notepad++.

Though the application overall is stable and usable, it should not be considered safe for critically important work.

There are numerous bugs and half working implementations. Pull requests are greatly appreciated.

![screenshot](/doc/screenshot.png)

# Installation

Packages are available for Windows, Linux, and MacOS.

Below are the supported distribution mechanisms. There may be other ways to download/install the application, but this project will likely not be able to offer any support for those since they are made availble by other individuals.

## Windows
Windows packages are available as an installer or a stand-alone zip file on the [release](https://github.com/dail8859/NotepadNext/releases) page. The installer provides additional components such as an auto-updater and Windows context menu integration. You can easily install it with Winget:

```powershell
winget install dail8859.NotepadNext
```

## Linux
Linux packages can be obtained by downloading the stand-alone AppImage on the [release](https://github.com/dail8859/NotepadNext/releases) page or by installing the [flatpak](https://flathub.org/apps/details/com.github.dail8859.NotepadNext) by executing:

```bash
flatpak install flathub com.github.dail8859.NotepadNext
```

## MacOS
MacOS disk images can be downloaded from the [release](https://github.com/dail8859/NotepadNext/releases) page.

It can also be installed using brew:
```bash
brew tap dail8859/notepadnext
brew install --no-quarantine notepadnext
```

#### MacOS Tweaks

By default, MacOS enables font smoothing which causes text to appear quite differently from the Windows version. This can be disabled system-wide using the following command:

```bash
defaults -currentHost write -g AppleFontSmoothing -int 0
```

A restart is required for this to take effect.

# Development
Current development is done using QtCreator with the Microsft Visual C++ (msvc) compiler. Qt 6.5 is the prefered Qt version but this can also be built with Qt 5.15. This is also known to build successfully on various Linux distributions and macOS. Other platforms/compilers should be usable with minor modifications.

If you are familiar with building C++ Qt desktop applications with Qt Creator, then this should be as simple as opening `src/NotepadNext.pro` and build/run the project.

If you are new to building C++ Qt desktop applications, there is a more detailed guide [here](/doc/Building.md).


# License
This code is released under the [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.txt).
