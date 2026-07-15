# 前置检查

## 系统要求

| 项目 | 要求 |
|------|------|
| 操作系统 | Windows 10+ (x64) |
| 磁盘空间 | ~15 GB (Qt SDK 较大) |
| 网络 | 需要下载 Qt (~2GB) 和第三方依赖 |
| Git | 系统需安装 Git |

## 检查清单

运行以下命令确认基础环境：

```cmd
python --version          # Python 3.8+
git --version             # Git 已安装
where python               # Python 在 PATH 中
```

如果 Python 未安装，从 [python.org](https://www.python.org/downloads/) 下载安装，安装时勾选「Add Python to PATH」。

## 代理配置（公司网络）

如果公司网络需要代理访问 GitHub，提前配置：

```cmd
set HTTP_PROXY=http://proxy.example.com:8080
set HTTPS_PROXY=http://proxy.example.com:8080
git config --global http.proxy http://proxy.example.com:8080
```

## 网络连通性测试

```cmd
ping github.com
```

如果 GitHub 直连不通，可使用镜像或代理。`aqtinstall` 会自动选择 Qt 镜像站点。