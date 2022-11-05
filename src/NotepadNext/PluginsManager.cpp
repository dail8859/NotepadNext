#include "PluginsManager.h"

#include <algorithm>

#include <QPluginLoader>
#include <QDir>
#include <QDebug>

PluginsManager *PluginsManager::_instance = nullptr;

PluginsManager::PluginsManager(QObject *parent) : QObject(parent)
{
}

PluginsManager::~PluginsManager()
{
    unloadPlugins();

    if (PluginsManager::instance())
    {
        delete PluginsManager::instance();
        PluginsManager::_instance = nullptr;
    }
}

PluginsManager *PluginsManager::instance() // 创建插件管理器
{
    if (_instance == nullptr)
        _instance = new PluginsManager();
    return _instance;
}

int PluginsManager::loadPlugins(const QString &folderPath)
{
    return loadPlugins(folderPath,"");
}

int PluginsManager::loadPlugins(const QString &folderPath, const QString /*fileExtension*/)
{
    // TODO: 未来会获得当前文件夹及其子文件夹下的所有dll文件,
    // 当前只能识别当前文件夹下的dll
    QDir pluginsDir(folderPath);

    QFileInfoList pluginsInfo = pluginsDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    //加载插件
    foreach (QFileInfo fileInfo, pluginsInfo)
    {
        qDebug() << fileInfo.absoluteFilePath();
        loadPluginFromPath(fileInfo.absoluteFilePath());
    }
    return 0;
}

void PluginsManager::unloadPlugins()
{
    foreach (QString filePath, _loaders.keys())
        unloadPluginFromPath(filePath);
}

int PluginsManager::loadPluginFromPath(const QString &filePath)
{
    if (!QLibrary::isLibrary(filePath))
        return -1;

    //加载插件
    QPluginLoader *loader = new QPluginLoader(filePath);
    if (loader->load())
    {
        PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
        if (plugin)
        {
            _plugins.push_back(plugin);
            _loaders.insert(filePath, loader);
            qDebug() << "add plugin: " << plugin->name();
            return 0;
        }
        else
        {
            delete loader;
            loader = nullptr;
        }
    }
    else
    {
        qDebug() << "loadPlugin:" << filePath << loader->errorString();
    }
    return -1;
}

int PluginsManager::unloadPluginFromPath(const QString &filePath)
{
    QPluginLoader *loader = _loaders.value(filePath);
    PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
    if (plugin)
    {
        _plugins.erase(std::remove(_plugins.begin(), _plugins.end(), plugin), _plugins.end());
    }

    //卸载插件，并从内部数据结构中移除
    if (loader->unload())
    {
        _loaders.remove(filePath);
        delete loader;
        loader = nullptr;
    }
    return 0;
}
