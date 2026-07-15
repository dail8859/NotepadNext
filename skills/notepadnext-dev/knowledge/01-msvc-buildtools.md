# MSVC BuildTools 安装

## 为什么不需要 Visual Studio IDE

NotepadNext 的 CMakeLists.txt 仅依赖 MSVC 编译器（`cl.exe`），不需要 Visual Studio IDE。
BuildTools 提供完整的 C++ 工具链，体积更小（~4GB vs IDE 的 ~20GB）。

## 安装步骤

### 1. 下载

访问 [Visual Studio BuildTools 下载页](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)，
下载「**Build Tools for Visual Studio 2022**」。

### 2. 安装

运行安装程序，在「**工作负载**」选项卡中选择：

- ✅ **Desktop development with C++**（C++ 桌面开发）

在右侧「安装详细信息」中确保以下组件选中：

- MSVC v143 - VS 2022 C++ x64/x86 build tools
- Windows 10/11 SDK
- CMake C++ tools for Windows（可选，便于 VS 集成）

点击「安装」，等待完成（约 10-20 分钟）。

### 3. 验证安装

打开一个新的 cmd 窗口：

```cmd
REM 初始化编译环境
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

REM 验证编译器
cl.exe
```

正常输出应显示编译器版本信息。

> **注意**：BuildTools 的安装路径会因 VS 版本而异：
> - VS 2022: `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\`
> - VS 2026+: `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\`
>
> 下面的脚本中已使用通配路径查找，你也可以运行 `vswhere -latest -property installationPath` 来定位安装路径。

### 4. 寻找 vcvars64.bat（脚本中自动处理）

如果 vcvars 路径不是标准位置，可以自动查找：

```python
import subprocess, os, glob

# 方法1：通过 vswhere 查找
vs_path = r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
if os.path.exists(vs_path):
    result = subprocess.run([vs_path, '-latest', '-property', 'installationPath'],
                           capture_output=True, text=True)
    install_path = result.stdout.strip()
    vcvars = install_path + r"\VC\Auxiliary\Build\vcvars64.bat"
    if os.path.exists(vcvars):
        print(f"vcvars64.bat: {vcvars}")
```

## 常见问题

### 安装程序报错

如果安装程序失败，尝试：
1. 以管理员身份运行
2. 关闭杀毒软件
3. 使用 `--layout` 参数离线下载后再安装

### 缺少 Windows SDK

如果编译时提示找不到 Windows SDK，回到 BuildTools 安装程序，在「单个组件」中勾选：
- Windows 10 SDK (10.0.xxxxx.x) 或 Windows 11 SDK