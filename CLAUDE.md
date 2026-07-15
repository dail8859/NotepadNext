# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

NotepadNext is a cross-platform reimplementation of Notepad++ using C++20 and Qt 6. It uses the Scintilla editing component (same engine as Notepad++) with Lua-based language definitions for syntax highlighting. Licensed GPLv3.

## Build Commands

**Prerequisites:** CMake 3.21+, Qt 6.5+ (6.5 recommended), C++20 compiler. Dependencies are auto-fetched via CPM (CMake Package Manager) on first build.

```bash
# Linux (Ubuntu/Debian)
sudo apt install cmake ninja-build qt6-base-dev qt6-base-dev-tools qt6-tools-dev \
  qt6-tools-dev-tools libqt6core5compat6-dev qt6-base-private-dev libxkbcommon-dev \
  libxcb-cursor-dev libcups2-dev libglib2.0-dev libproxy-dev

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# AppImage (Linux)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DAPP_DISTRIBUTION=AppImage
cmake --build build --target appimage --parallel

# Windows (MSVC 2022 + Qt 6.5)
# Open in Qt Creator → configure for "Desktop Qt 6.5 MSVC2019 64bit" → Ctrl+R
```

CPM caches sources in `.cpm-cache/` at the repo root.

**There is no test suite.** The only automated quality check is `codespell` on `./src` (skipping `.lua` and `.ui` files).

## Architecture

### Core Application Flow

- **`main.cpp`** → Constructs `NotepadNextApplication` (inherits `SingleApplication`). If primary instance, calls `init()` which sets up the full GUI. If secondary instance, sends file paths to primary via IPC and exits.
- **`NotepadNextApplication`** — Central coordinator. Owns: `EditorManager`, `SessionManager`, `RecentFilesListManager`, `TranslationManager`, `LuaState`, `ApplicationSettings`, `MainWindow`. Auto-saves session every 60 seconds.
- **`ScintillaNext`** — Core editor widget wrapping Scintilla. Custom regex search via `QRegexSearch` (enabled by `SCI_OWNREGEX` compile define).
- **`MainWindow`** (`dialogs/MainWindow.cpp`, ~2400 lines) — The largest source file. Primary UI container with all menus, toolbars, and dock management.

### Decorator Pattern

Editor behaviors are modular decorators in `src/decorators/`. Two base classes:
- **`EditorDecorator`** — per-editor behaviors (AutoCompletion, BraceMatch, LineNumbers, SmartHighlighter, etc.)
- **`ApplicationDecorator`** — app-wide behaviors (EditorConfigAppDecorator, MarkerAppDecorator)

Each decorator attaches to an editor instance and can be enabled/disabled independently.

### Language System

87 Lua files in `src/languages/` define syntax highlighting (lexer name, extensions, keywords, styles). Loaded at startup via `src/scripts/init.lua`. Language detection goes: file extension → file contents → fallback to "Text".

`SciIFaceTable.cpp` (125KB) and `ScintillaEnums.h` (35KB) are generated from Scintilla's interface definitions — don't hand-edit. See `scripts/GenerateEnums.py`.

### Single-Instance IPC

Uses [itay-grudev/SingleApplication](https://github.com/itay-grudev/SingleApplication) (v3.5.2, pinned commit in `thirdparty/CMakeLists.txt`). When a secondary instance launches:
1. Constructor handshake emits `instanceStarted` on primary → `bringWindowToForeground()`
2. `sendInfoToPrimaryInstance()` serializes file paths via `sendMessage()` (default 100ms timeout, no retry)
3. Primary's `receivedMessage` signal → `receiveInfoFromSecondaryInstance()` → `openFiles()`

The `instanceStarted` and `receivedMessage` signals are independent — window activation can succeed while file delivery fails. This is a known reliability issue (see GitHub issue #541).

### Third-Party Dependencies

| Library | Source | Purpose |
|---------|--------|---------|
| Scintilla + Lexilla | `thirdparty/` (bundled) | Core text editing engine + lexers |
| Lua | `thirdparty/lua/` (bundled) | Scripting engine for language definitions |
| uchardet | `thirdparty/uchardet/` (bundled) | Character encoding detection |
| Qt-Advanced-Docking-System | CPM (GitHub) | Tabbed/docked editor panels |
| QSimpleUpdater | CPM (GitHub) | Auto-update framework |
| SingleApplication | CPM (GitHub) | Single-instance with IPC |
| editorconfig-core-qt | CPM (GitHub) | .editorconfig support |

### Source Layout

```
src/
├── main.cpp, NotepadNextApplication.*     # Entry point, app coordinator
├── ScintillaNext.*, EditorManager.*       # Core editor
├── Finder.*, QRegexSearch.*               # Search/replace
├── LuaState.*, LuaExtension.*            # Lua scripting bridge
├── Macro*.*, SessionManager.*             # Macro system, session persistence
├── decorators/                            # Modular editor behaviors (15 decorators)
├── dialogs/                               # MainWindow, FindReplace, Preferences, etc.
├── docks/                                 # Dock panels (FileList, Search Results, Lua Console, etc.)
├── widgets/                               # Custom widgets (status bar, quick find, tab bar)
├── languages/                             # 87 Lua language definition files
├── scripts/init.lua                       # Lua startup script
└── icons/, stylesheets/                   # UI assets
```

## Key Patterns

- **Build defines:** `QAPPLICATION_CLASS=QApplication`, `ADS_STATIC`, `QSIMPLEUPDATER_STATIC`, `APP_VERSION`, `APP_DISTRIBUTION`
- **Platform-specific code** is guarded by `#ifdef Q_OS_WIN` / `Q_OS_MACOS` — notably `MainWindow::bringWindowToForeground()` uses Win32 API on Windows.
- **Qt resources:** `resources.qrc` (icons/stylesheets), `scripts.qrc` (Lua scripts and language files). Translations in `i18n/` are compiled via `qt_add_translations()`.
- **Security hardening:** MSVC gets `/guard:cf` (Control Flow Guard). Linux gets `-fstack-protector-strong`, `FORTIFY_SOURCE=2`, full RELRO.
- **Scintilla updates:** Follow the 4-step process in `doc/Update Scintilla.md`.
