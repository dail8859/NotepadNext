# Qt SDK 安装

## 安装方式对比

| 方式 | 优点 | 缺点 |
|------|------|------|
| **aqtinstall（推荐）** | 无需注册，命令行一键安装，体积小 | 不包含 Qt Creator |
| Qt Online Installer | 官方支持，包含所有组件 | 需要注册 Qt 账号，体积大 |
| Qt Maintenance Tool | 易于管理版本 | 同上 |

## 推荐：使用 aqtinstall

### 1. 安装 aqt

```cmd
python -m pip install aqtinstall
```

### 2. 选择 Qt 版本

**版本匹配规则**：

| MSVC 编译器 | 推荐 Qt 版本 |
|------------|-------------|
| MSVC 2022 (v17.x) / 2026 (v18.x) | Qt 6.8.x |
| MSVC 2019 (v16.x) | Qt 6.5.x |
| MSVC 2017 (v15.x) | Qt 6.2.x |

查看可用的 Qt 版本和架构：

```cmd
REM 列出可用版本
python -m aqt list-qt windows desktop

REM 列出某个版本可用的架构
python -m aqt list-qt windows desktop --arch 6.8.2

REM 列出可用的模块
python -m aqt list-qt windows desktop --modules 6.8.2 win64_msvc2022_64
```

关键输出：
```
win64_mingw          MinGW 编译版（不需要 VS）
win64_msvc2022_64    MSVC 2022 64位（匹配你的 MSVC 2022/2026）
win64_msvc2019_64    MSVC 2019 64位
```

### 3. 安装

```cmd
python -m aqt install-qt windows desktop 6.8.2 win64_msvc2022_64 ^
    -O C:\Qt ^
    --modules qt5compat
```

参数说明：
- `6.8.2` — Qt 版本
- `win64_msvc2022_64` — MSVC 2022 64位架构
- `-O C:\Qt` — 安装到 `C:\Qt\6.8.2\msvc2022_64\`
- `--modules qt5compat` — 额外安装 Qt5 兼容模块（NotepadNext 需要）

### 4. 验证

```cmd
C:\Qt\6.8.2\msvc2022_64\bin\qmake.exe --version
```

应输出：
```
QMake version 3.1
Using Qt version 6.8.2 in C:/Qt/6.8.2/msvc2022_64/lib
```

## 方式 B：Qt Online Installer

如果 aqt 安装失败或需要安装其他组件（如 Qt Creator），可以用官方安装器：

1. 下载 [Qt Online Installer](https://www.qt.io/download-qt-installer)
2. 注册/登录 Qt 账号
3. 自定义安装 → 选择以下组件：
   - Qt 6.8.x → `MSVC 2022 64-bit`
   - Qt → `Qt 5 Compatibility Module`
   - Developer and Design Tools → `Qt Creator`（可选）
4. 完成安装

## Qt 安装目录结构

安装完成后的典型目录：

```
C:\Qt\6.8.2\msvc2022_64\
├── bin\
│   ├── qmake.exe          # 构建工具
│   ├── moc.exe             # Meta-Object Compiler
│   ├── rcc.exe             # Resource Compiler
│   ├── windeployqt6.exe    # 部署工具（打包用）
│   └── Qt6Core.dll         # 运行时库
├── include\                 # 头文件
├── lib\                     # 链接库
│   └── cmake\               # CMake 配置（CMAKE_PREFIX_PATH 指向此处）
└── plugins\                 # 平台插件
```

## 多版本共存

如果已安装了多个 Qt 版本（如 6.5.3 + 6.8.2），CMake 会使用 `CMAKE_PREFIX_PATH` 或 `Qt6_DIR` 指定的版本。通过设置不同的环境变量来切换：

```cmd
REM 使用 Qt 6.8.2
set CMAKE_PREFIX_PATH=C:\Qt\6.8.2\msvc2022_64

REM 使用 Qt 6.5.3
set CMAKE_PREFIX_PATH=C:\Qt\6.5.3\msvc2019_64
```