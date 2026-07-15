# 项目克隆与构建

## 1. 克隆项目

```cmd
git clone https://github.com/dail8859/NotepadNext.git
cd NotepadNext
```

## 2. 复制 build_and_package.bat

将 `build_and_package.bat` 复制到项目根目录（即 `CMakeLists.txt` 同级）。

> 此脚本位于本 skill 所在仓库的 `skills/notepadnext-dev/assets/build_and_package.bat`。

## 3. 一键构建

```cmd
build_and_package.bat              # 增量编译 + 打包
build_and_package.bat --clean      # 全量重编译
```

脚本会自动完成：
- 初始化 MSVC 编译环境
- CMake 配置（首次）
- 并行编译
- Qt DLLs 部署 (windeployqt6)
- 打包到 `dist\` 目录

产物在 `dist\NotepadNext.exe`，双击即可运行。

## 4. 手动分步构建

如果一键脚本不适用，可以手动分步：

```cmd
REM 开新 cmd 窗口
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set CMAKE_PREFIX_PATH=C:\Qt\6.8.2\msvc2022_64
set Qt6_DIR=C:\Qt\6.8.2\msvc2022_64\lib\cmake\Qt6

REM 配置（仅首次）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

REM 编译
cmake --build build --config Release --parallel

REM 部署 Qt DLLs
C:\Qt\6.8.2\msvc2022_64\bin\windeployqt6.exe --release --no-translations build\Release\NotepadNext.exe

REM 创建 qt.conf
echo [Paths] > build\Release\qt.conf
echo Prefix = . >> build\Release\qt.conf
echo Plugins = . >> build\Release\qt.conf
```

## 5. 项目结构概览

```
NotepadNext/
├── CMakeLists.txt               # 顶层 CMake
├── src/                          # NotepadNext 主源码
│   ├── main.cpp
│   ├── NotepadNextApplication.cpp   # ← SingleApplication 入口
│   ├── MainWindow.cpp
│   └── ...
├── thirdparty/                   # 本地引入的第三方库
│   ├── scintilla/                # 编辑器内核
│   ├── lexilla/                  # 语法高亮
│   ├── lua/                      # Lua 脚本引擎
│   └── uchardet/                 # 编码检测
├── .cpm-cache/                   # CPM 远程第三方（构建时自动下载）
│   ├── singleapplication/        # ← 单实例 IPC 库
│   ├── ads/                      # Dock 布局系统
│   ├── qsimpleupdater/           # 自动更新
│   └── editorconfig_core_qt/     # EditorConfig 支持
├── build/                        # 构建产物
│   └── Release/
│       ├── NotepadNext.exe
│       ├── Qt6*.dll
│       └── qt.conf
├── dist/                         # 打包输出目录
├── build_and_package.bat         # 一键构建脚本
└── doc/
    └── Building.md               # 官方构建文档
```

## 6. 修改 recompile 流程

每次改代码后重新编译：

```cmd
build_and_package.bat    # 增量编译，约 1-3 分钟
```

只会重新编译改动的源文件。需要全量重编时加 `--clean`。

### 只编译不打包

```cmd
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cmake --build build --config Release --parallel
```

编译后的 exe 在 `build\Release\NotepadNext.exe`（可以直接运行，生成时 Qt DLLs 已在原地，不需要每次重新打包）。

## 注意事项

- MSVC 编译器路径随 VS 版本变化。如果你的 `vcvars64.bat` 不在 `18\BuildTools` 下，修改 `build_and_package.bat` 中的 `VCVARS` 变量
- 首次构建会通过 CPM 下载第三方依赖（~50MB），需要能访问 GitHub
- CPM 缓存目录 `.cpm-cache` 可以被多个项目共享（`--clean` 不会删除它）
- dist 目录是自包含的完整可运行包，可以直接复制到其他机器使用