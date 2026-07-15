@echo off
setlocal
title NotepadNext Build

REM ============================================================
REM  NotepadNext Build & Package Script (portable)
REM
REM  Auto-detects MSVC and Qt installations.
REM  Override with environment variables:
REM    VCVARS_PATH    - path to vcvars64.bat
REM    QT_ROOT        - path to Qt root (e.g. C:\Qt\6.8.2\msvc2022_64)
REM ============================================================
set "PROJECT_ROOT=%~dp0"
if "%PROJECT_ROOT:~-1%"=="\" set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"
set "BUILD_DIR=%PROJECT_ROOT%\build"
set "RELEASE_DIR=%BUILD_DIR%\Release"
set "DIST_DIR=%PROJECT_ROOT%\dist"

REM --- Auto-detect MSVC ---
if not defined VCVARS_PATH (
    echo [INFO] Auto-detecting MSVC BuildTools...
    set "VCTEMP=%TEMP%\vs_instpath.txt"
    powershell -NoProfile -Command ^
        "$vswhere = Get-ChildItem -Path 'C:\Program Files (x86)\Microsoft Visual Studio\Installer' -Filter 'vswhere.exe' -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1; if ($vswhere) { $path = & $vswhere.FullName -latest -property installationPath; if ($path) { $bat = Join-Path $path 'VC\Auxiliary\Build\vcvars64.bat'; if (Test-Path $bat) { $bat | Out-File '%VCTEMP%' -Encoding utf8 } } }" > NUL 2>&1
    if exist "%VCTEMP%" (
        set /p VCVARS_PATH=< "%VCTEMP%"
        del "%VCTEMP%" 2> NUL
    ) else (
        for %%d in ("18" "2022" "2019") do (
            if exist "C:\Program Files (x86)\Microsoft Visual Studio\%%~d\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
                set "VCVARS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%%~d\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
            )
        )
    )
)
if not defined VCVARS_PATH (
    echo [FAIL] Cannot find vcvars64.bat! Set VCVARS_PATH or install VS BuildTools.
    exit /b 1
)
echo [INFO] vcvars: %VCVARS_PATH%

REM --- Auto-detect Qt ---
if not defined QT_ROOT (
    echo [INFO] Auto-detecting Qt SDK...
    for /d %%d in (C:\Qt\*) do (
        for /d %%a in (%%d\*) do (
            if exist "%%a\bin\windeployqt6.exe" (
                set "QT_ROOT=%%a"
            )
        )
    )
)
if not defined QT_ROOT (
    echo [FAIL] Cannot find Qt SDK! Set QT_ROOT or install Qt.
    exit /b 1
)
echo [INFO] Qt:     %QT_ROOT%

echo.
echo ============================================================
echo   NotepadNext Build ^& Package
echo ============================================================
echo   Source:  %PROJECT_ROOT%
echo   Output:  %DIST_DIR%
echo ============================================================
echo.

REM --- Check for --clean flag ---
if /i "%1"=="--clean" (
    echo [CLEAN] Removing build and dist directories...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    if exist "%DIST_DIR%" rmdir /s /q "%DIST_DIR%"
    echo [ OK ] Cleaned
)

REM --- Step 1: Initialize MSVC environment ---
echo [1/5] Initializing MSVC compiler...
call "%VCVARS_PATH%" > NUL 2>&1 || (
    echo [FAIL] vcvars64.bat failed ^!
    exit /b 1
)
echo [ OK ] MSVC compiler ready

REM --- Step 2: CMake Configure ---
echo [2/5] Configuring CMake...
set "CMAKE_PREFIX_PATH=%QT_ROOT%"
set "Qt6_DIR=%QT_ROOT%\lib\cmake\Qt6"

if exist "%BUILD_DIR%\CMakeCache.txt" (
    echo [SKIP] CMakeCache.txt found, reusing cache.
    echo        Run with --clean to force reconfigure.
) else (
    cmake -S "%PROJECT_ROOT%" -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Release || (
        echo [FAIL] CMake configure failed ^!
        exit /b 1
    )
    echo [ OK ] CMake configure complete
)

REM --- Step 3: Build ---
echo [3/5] Building Release...
cmake --build "%BUILD_DIR%" --config Release --parallel || (
    echo [FAIL] Build failed ^!
    exit /b 1
)
echo [ OK ] Build complete

REM --- Step 4: Deploy Qt DLLs and create qt.conf ---
echo [4/5] Deploying dependencies...
set "PATH=%QT_ROOT%\bin;%PATH%"
"%QT_ROOT%\bin\windeployqt6.exe" ^
    --release ^
    --no-translations ^
    --no-system-d3d-compiler ^
    --no-opengl-sw ^
    "%RELEASE_DIR%\NotepadNext.exe" > NUL 2>&1

> "%RELEASE_DIR%\qt.conf" (
    echo [Paths]
    echo Prefix = .
    echo Plugins = .
)
echo [ OK ] Qt DLLs deployed, qt.conf created

REM --- Step 5: Package to dist/ ---
echo [5/5] Packaging to dist...
if exist "%DIST_DIR%" rmdir /s /q "%DIST_DIR%"
xcopy "%RELEASE_DIR%" "%DIST_DIR%\" /E /I /Q /Y > NUL
echo [ OK ] Packaged

REM --- Summary ---
echo.
echo ============================================================
echo   Build SUCCESS ^!
echo ============================================================
echo   Run:  %DIST_DIR%\NotepadNext.exe
echo.
echo   Hint: %~nx0 --clean   (full rebuild)
echo ============================================================

endlocal