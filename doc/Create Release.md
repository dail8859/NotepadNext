## Prepare Release

- Update version in `src/Version.pri`
- Add new `<release>` tag to metainfo file in `deploy/linux`
- Commit
- Tag commit with new version number
- Push to commit and tag to GitHub
- Once GitHub action finishes download Qt5 outputs for each platform
- Extract each and attach a new release

## Manually Build Release
Example bat script to build release

```
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
set PATH=C:\Qt\5.15.2\msvc2019_64\bin\;C:\Qt\Tools\QtCreator\bin\jom\;%PATH%
set PATH=C:\Program Files\7-Zip;%PATH%
set PATH=C:\Program Files (x86)\NSIS\Bin;%PATH%
mkdir build
cd build
qmake ..\src\NotepadNext.pro
jom
jom package
jom zip
jom installer
cd ..
copy build\NotepadNext*.zip .
copy installer\NotepadNext*.exe .
```