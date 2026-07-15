# 故障排查

## 编译错误

### `stdext": 未声明的标识符`

**现象**：
```
error C2065: "stdext": 未声明的标识符
qvarlengtharray.h(379,19): error C3861: "stdext": 找不到标识符
```

**原因**：Qt 版本和 MSVC 编译器版本不匹配。Qt 6.5.x 是为 MSVC 2019 编译的，在 MSVC 2022/2026 上 `stdext` 命名空间已移除。

**解决**：安装匹配 MSVC 版本的 Qt。MSVC 2022+ 用 Qt 6.8.x：

```cmd
python -m aqt install-qt windows desktop 6.8.2 win64_msvc2022_64 -O C:\Qt --modules qt5compat
```

### `CMAKE_PREFIX_PATH` 找不到 Qt

**现象**：
```
Could not find a package configuration file provided by "QT" with any of the following names:
  Qt6Config.cmake
  qt6-config.cmake
```

**原因**：`CMAKE_PREFIX_PATH` 没有正确指向 Qt 根目录。

**修复**：
```cmd
REM 正确：指向 Qt 根目录（不是 cmake 子目录）
set CMAKE_PREFIX_PATH=C:\Qt\6.8.2\msvc2022_64

REM 错误：不要指向 cmake 子目录
REM set CMAKE_PREFIX_PATH=C:\Qt\6.8.2\msvc2022_64\lib\cmake
```

### `vswhere.exe not found`

**现象**：运行 vcvars 时出现此警告。

**结论**：**无害**。此警告不影响编译器初始化。vcvars64.bat 仍能正常配置环境。

### `WrapVulkanHeaders not found`

**现象**：CMake 配置时提示找不到 Vulkan。

**结论**：**无害**。NotepadNext 不使用 Vulkan，此检查失败不影响编译。

### windeployqt 找不到 `VCINSTALLDIR`

**现象**：
```
Warning: Cannot find Visual Studio installation directory, VCINSTALLDIR is not set.
```

**原因**：运行 windeployqt 的 cmd 窗口没有执行过 vcvars。

**修复**：在新 cmd 窗口中先运行 vcvars 再执行 windeployqt（或使用 `build_and_package.bat`）：

```cmd
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
windeployqt6.exe --release --no-translations build\Release\NotepadNext.exe
```

## 运行时问题

### 双击 exe 无任何反应

**原因**：缺少 Qt DLLs 或 qt.conf 配置不正确。

**检查**：
1. 确保 `build\Release\` 下有 `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll` 等
2. 确保 `qt.conf` 文件存在且内容为：
   ```ini
   [Paths]
   Prefix = .
   Plugins = .
   ```
3. 确保 `platforms\qwindows.dll` 等插件目录存在

**修复**：重新运行 `build_and_package.bat`，它会自动执行 windeployqt6 和创建 qt.conf。

### 程序启动后立即退出

在 cmd 中运行，可以看到标准错误输出：

```cmd
cd build\Release
NotepadNext.exe
```

常见原因：
- Qt 版本和 DLL 不匹配（build 目录下残留旧版本 DLL）
- 缺少 OpenSSL 库（如果使用了 `--no-translations` 但启用了 HTTPS 功能则需要）

### 单实例 IPC 不可靠（本仓库要修复的目标）

**现象**：已有 NotepadNext 实例运行时，从文件管理器右键「Edit with Notepad Next」打开文件，窗口被激活但文件未打开。

**原因**：SingleApplication 库的 `sendMessage()` 在 QLocalServer 忙时超时。

**相关文件**：
- `src/NotepadNextApplication.cpp` → `sendInfoToPrimaryInstance()`
- `.cpm-cache/singleapplication/91c3/singleapplication_p.cpp` → 底层 IPC 实现

## 网络相关

### CPM 下载第三方库失败

**现象**：CMake 配置时 CPM 无法下载 GitHub 依赖。

**解决**：
1. 配置 Git 代理：`git config --global http.proxy http://proxy:port`
2. 手动下载后放入 `.cpm-cache/` 目录
3. 确保能访问 `github.com` 和 `raw.githubusercontent.com`

### aqt 下载 Qt 失败

**现象**：`aqtinstall` 各种超时或 404。

**解决**：
1. aqt 会自动尝试多个镜像，等待即可
2. 如果所有镜像都不通，手动指定镜像：
   ```cmd
   set AQT_MIRROR=https://qt.mirror.constant.com
   ```
3. 也可下载 Qt 离线安装包从 [Qt Archive](https://download.qt.io/archive/qt/)