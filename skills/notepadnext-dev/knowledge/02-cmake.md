# CMake 安装

## 推荐方式：Zip 包直接部署

CMake 的 zip 包无需安装，解压即可使用，方便迁移。

### 1. 下载

从 [CMake Releases](https://github.com/Kitware/CMake/releases) 下载最新的 `cmake-*-windows-x86_64.zip`。

示例（当前最新稳定版 4.2.1）：
```
https://github.com/Kitware/CMake/releases/download/v4.2.1/cmake-4.2.1-windows-x86_64.zip
```

### 2. 解压

解压到 `C:\cmake`，确保路径结构为：

```
C:\cmake\
└── cmake-4.2.1-windows-x86_64\
    ├── bin\
    │   └── cmake.exe
    ├── share\
    └── ...
```

### 3. 加入 PATH

**方法 A：通过系统设置（推荐）**

1. 按 `Win + R`，输入 `sysdm.cpl`
2. 高级 → 环境变量
3. 在「用户变量」中找到 `PATH`，添加：`C:\cmake\cmake-4.2.1-windows-x86_64\bin`

**方法 B：通过命令行**

```cmd
setx PATH "C:\cmake\cmake-4.2.1-windows-x86_64\bin;%PATH%"
```

> 注意：setx 有长度限制，PATH 过长时可能截断，推荐用方法 A。

### 4. 验证

打开新的 cmd 窗口：

```cmd
cmake --version
```

应输出类似：
```
cmake version 4.2.1
```

## 备选方式：MSI Installer

如果 zip 方式不方便，也可以下载 `.msi` 安装包，安装时勾选「Add CMake to the system PATH」。

## 备选方式：winget

```cmd
winget install Kitware.CMake
```

> 注意：winget 在某些版本的 Windows 中可能不可用，需要先通过 Microsoft Store 安装 App Installer。