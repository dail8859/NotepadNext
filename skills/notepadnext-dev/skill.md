---
name: notepadnext-dev
description: NotepadNext 开发环境搭建和编译。包括 MSVC BuildTools、CMake、Qt SDK 等工具的安装，以及从源码编译、打包的完整流程。适用于在新机器上快速建立 NotepadNext 开发环境。
allowed-tools: Bash, Read, Write, Edit, Grep, WebFetch, Task
---

# NotepadNext 开发环境搭建

## Overview

本 Skill 提供 NotepadNext 开发编译环境的完整搭建流程：

- MSVC BuildTools 检查与安装（无需 Visual Studio IDE）
- CMake 安装
- Qt 6.x SDK 安装（通过 aqt，无需注册 Qt 账号）
- 项目克隆与构建
- 一键 build & package 脚本

**适用场景**：新机器初始化编译环境、CI/CD 构建节点、离线环境前的准备

## 前置检查

开始前运行 [knowledge/00-prerequisites.md](knowledge/00-prerequisites.md) 确认基础环境。

## 一键搭建流程

在新机器上按以下步骤操作：

### Step 1: MSVC BuildTools

必须安装 **Visual Studio BuildTools**（C++ 桌面开发工作负载），不需要 VS IDE。

详见 [knowledge/01-msvc-buildtools.md](knowledge/01-msvc-buildtools.md)。

关键点：
- 下载 [Visual Studio BuildTools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)
- 安装时选择「**Desktop development with C++**」工作负载
- 安装完成后验证：确认 `vcvars64.bat` 存在且 `cl.exe` 可用

### Step 2: CMake

下载 zip 包直接解压到 `C:\cmake`，然后加入 PATH。

详见 [knowledge/02-cmake.md](knowledge/02-cmake.md)。

关键点：
- 从 [CMake Releases](https://github.com/Kitware/CMake/releases) 下载 `cmake-*-windows-x86_64.zip`
- 解压到 `C:\cmake`
- 将 `C:\cmake\cmake-*-windows-x86_64\bin` 加入用户 PATH

### Step 3: Qt SDK

通过 `aqtinstall` 工具下载 Qt，无需 Qt 账号。

详见 [knowledge/03-qt-sdk.md](knowledge/03-qt-sdk.md)。

关键点：
- `pip install aqtinstall`
- 使用 `aqt list-qt` 查看可用 MSVC 版本
- 安装：`aqt install-qt windows desktop <version> win64_msvc2022_64 -O C:\Qt --modules qt5compat`
- 版本选择规则：Qt 版本需匹配 MSVC 版本。MSVC 2022/2026 用 Qt 6.8.x；MSVC 2019 用 Qt 6.5.x

### Step 4: 克隆项目并构建

详见 [knowledge/04-build.md](knowledge/04-build.md)。

关键点：
- 复制 `build_and_package.bat` 到项目根目录
- 双击运行即可完成构建+打包

## 构建命令速查

```cmd
REM 初始化（开新 cmd 窗口）
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set CMAKE_PREFIX_PATH=C:\Qt\6.8.2\msvc2022_64
set Qt6_DIR=C:\Qt\6.8.2\msvc2022_64\lib\cmake\Qt6

REM Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

REM Build
cmake --build build --config Release --parallel

REM Deploy Qt DLLs
C:\Qt\6.8.2\msvc2022_64\bin\windeployqt6.exe --release --no-translations build\Release\NotepadNext.exe

REM Package
xcopy build\Release dist\ /E /I /Q /Y
```

## 故障排查

常见问题详见 [knowledge/05-troubleshooting.md](knowledge/05-troubleshooting.md)：

- `stdext": 未声明的标识符` — Qt 版本和 MSVC 版本不匹配，换用新版 Qt
- `CMAKE_PREFIX_PATH` 找不到 Qt — 确保 `CMAKE_PREFIX_PATH` 指向 Qt 根目录（非 cmake 子目录）
- `vswhere.exe not found` — 无害警告，编译器仍正常工作
- windeployqt 找不到 `VCINSTALLDIR` — 需要在 vcvars 初始化的 cmd 环境中运行