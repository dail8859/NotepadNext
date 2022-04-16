# Building Notepad Next

To build Notepad Next you will need a compatible C++ compiler, the Qt libraries, and the Notepad Next source code.

# Windows

This section specifically describes how to build Notepad Next using Microsoft's Visual Studio 2019 compiler. 

## Installing Visual Studio 2019

1. Download the installer for [Visual Studio 2019 Community](https://visualstudio.microsoft.com/) (other versions should work if you have those installed already).
1. Run the installer
1. Select the 'Desktop development with C++'
1. Complete the installation

## Installing Qt Libraries

1. Download the [Qt installer](https://www.qt.io/download-qt-installer)
1. Run the installer.
1. Select 'custom installation'
1. Install any version of Qt >= 5.15:
  * `MSVC 2019 64-bit` located under the desired version of Qt
  * If Qt 6+ is being used, you must also install `Qt 5 Compatibility Module`
  * `Developer and Design Tools` > `Qt Creator CDB Debugger Support`
  * `Developer and Design Tools` > `Debugging Tools for Windows`

## Cloning the Notepad Next Repository

1. In a command prompt (or git shell, powershell, etc) run:
1. `git clone --recurse-submodules https://github.com/dail8859/NotepadNext.git`
1. `cd NotepadNext`


## Building/Running Notepad Next

1. Open `src/NotepadNext.pro` with Qt Creator
1. Configure the project for 'Desktop Qt 5.15.2 MSVC2019 64bit'
1. Press `Ctrl+R`
1. Qt Creator will build and run the project.

# Linux

Using a fresh Ubuntu 21.10 setup, the following script will install the needed dependencies and build the executable:

```
sudo apt install qtbase5-dev qt5-qmake qtbase5-dev-tools qtbase5-private-dev libqt5x11extras5-dev build-essential git
git clone --recurse-submodules https://github.com/dail8859/NotepadNext.git
cd NotepadNext
mkdir build
cd build
qmake ../src/NotepadNext.pro
make -j$(nproc)
```

If encountered `/usr/lib/qt5/bin/lrelease not found` error. Please install `qttools5-dev-tools`.
```
sudo apt-get install qttools5-dev-tools
```
# CMake support

To build this project using CMake (currently the official way is QMake), you should type this
on the command line (tested on Linux, but should work on Windows as well as OSX):

```
cmake -S . -B cbuild -G Ninja
cmake --build cbuild
```

If you are using Visual Studio - you can open the directory containing the source
and then use CMake directly on Visual Studio, no addons needed (as Visual Studio 2019
and above have really good support for CMake).

Using CLion should be similar - just open the directory inside CLion and you
should be ready to code.

Currently CMake uses CPM to pull all 3rd parties (originally 3rd parties where
a mix of submodules and in-repo code). The initial config should take a large time
(the main time consumer is cloning of scintilla).

A future goal is to move all 3rd party dependencies to conan or vcpkg (or some
other package manager). This will also give us pre-compiled 3rd parties which
will reduce compile times as we will use pre-compiled binaries - and will ease the
ability to gain build-reproducability.
