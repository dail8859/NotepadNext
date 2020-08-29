## Prepare Release

1. Update version in `appveyor.yml`
2. Update version in `installer/config/config.xml`
3. Update version and date in `installer/packages/app/meta/package.xml`
4. Update version in `src/Version.pri`

## Build Release
Example bat script to build release

```
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
set PATH=C:\Qt\5.15.0\msvc2019_64\bin\;C:\Qt\Tools\QtCreator\bin\;%PATH%
mkdir build
cd build
qmake ..\src\NotepadNext.pro
jom
jom make_package
jom zip
jom prepare_installer
cd ..
mkdir installer_build
cd installer_build
qmake ..\installer\installer.pro
jom
```