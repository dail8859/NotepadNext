"C:\Program Files\Inkscape\bin\inkscape.exe" -w 16 -h 16 --export-type="png" --export-filename="nn16.png" NotepadNext.svg
"C:\Program Files\Inkscape\bin\inkscape.exe" -w 32 -h 32 --export-type="png" --export-filename="nn32.png" NotepadNext.svg
"C:\Program Files\Inkscape\bin\inkscape.exe" -w 48 -h 48 --export-type="png" --export-filename="nn48.png" NotepadNext.svg
"C:\Program Files\Inkscape\bin\inkscape.exe" -w 64 -h 64 --export-type="png" --export-filename="nn64.png" NotepadNext.svg
"C:\Program Files\Inkscape\bin\inkscape.exe" -w 128 -h 128 --export-type="png" --export-filename="nn128.png" NotepadNext.svg
".\ImageMagick\magick" convert -background transparent nn16.png nn32.png nn48.png nn64.png nn128.png NotepadNext.ico

del nn16.png
del nn32.png
del nn48.png
del nn64.png
del nn128.png

"C:\Program Files\Inkscape\bin\inkscape.exe" --export-type="png" --export-dpi=96 --export-background-opacity=0 NotepadNext.svg

".\png2icons.exe" NotepadNext.png NotepadNext -icns

copy NotepadNext.png ..\src\NotepadNext\icons\NotepadNext.png
