# Building Notepad Next

To build Notepad Next you will need CMake, a compatible C++ compiler, the Qt libraries, and the Notepad Next source code.

# Windows

This section specifically describes how to build Notepad Next using Microsoft's Visual Studio 2022 compiler. 

## Installing Visual Studio 2022

1. Download the installer for [Visual Studio 2022 Community](https://visualstudio.microsoft.com/) (other versions should work if you have those installed already).
1. Run the installer
1. Select the 'Desktop development with C++'
1. Complete the installation

## Installing Qt Libraries

1. Download the [Qt installer](https://www.qt.io/download-qt-installer)
1. Run the installer.
1. Select 'custom installation'
1. Install any version of Qt >= 6.5 (6.5 is recommended if possible)
  * `MSVC 2019 64-bit` located under the desired version of Qt
  * `Qt 5 Compatibility Module` (not needed for Qt 5.x)
  * `Developer and Design Tools` > `Qt Creator CDB Debugger Support`
  * `Developer and Design Tools` > `Debugging Tools for Windows`

## Cloning the Notepad Next Repository

1. In a command prompt (or git shell, powershell, etc) run:
1. `git clone https://github.com/dail8859/NotepadNext.git`

## Building/Running Notepad Next

1. Open Qt Creator and open the root `CMakeLists.txt` file.
1. Configure the project for 'Desktop Qt 6.5 MSVC2019 64bit' (or whatever version of Qt you have installed)
1. Press `Ctrl+R`
1. Qt Creator will build and run the project.

# Linux

TODO

# MacOS

TODO
