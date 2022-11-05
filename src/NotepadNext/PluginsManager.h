#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include "PluginInterface.h"

#include <vector>

#include <QObject>
#include <QHash>
#include <QPluginLoader>

class QPluginLoader;

/**
 * @brief 插件管理器
 *
 * @code
 * PluginsManager *pluginsManager = PluginsManager::instance();
 * pluginsManager->loadAllPlugins(R"(./plugins)");
 * @endcode
 *
 */
class PluginsManager : public QObject
{
    Q_OBJECT

private:
    explicit PluginsManager(QObject *parent = nullptr);
    ~PluginsManager();
    PluginsManager(const PluginsManager &rhs);
    const PluginsManager &operator=(const PluginsManager &rhs);

public:
    /**
     * @~Chinese
     * @brief 实例化插件管理器
     *
     * @return PluginManager*
     */
    static PluginsManager *instance();

    /**
     * @~Chinese
     * @brief 在指定文件夹下加载所有后缀为fileExtension的文件
     *
     * @param[in] folderPath 文件夹路径
     * @param[in] fileExtension 文件后缀
     *
     * @return int
     *  @retval 0 程序执行成功
     *  @retval -1 程序执行失败
     */
    int loadPlugins(const QString &folderPath, const QString /*fileExtension*/);
    int loadPlugins(const QString &folderPath = "./plugins");

    /**
     * @~Chinese
     * @brief 卸载所有插件
     *
     */
    void unloadPlugins();

    /**
     * @~Chinese
     * @brief 根据完整的文件路径(包括后缀名)加载插件
     *
     * @param[in] filePath 完整的文件路径 "./plugins/demo.dll"
     *
     * @return int
     *  @retval 0 程序执行成功
     *  @retval -1 程序执行失败
     */
    int loadPluginFromPath(const QString &filePath);

    /**
     * @~Chinese
     * @brief 根据完整的文件路径(包括后缀名)卸载插件
     *
     * @param[in] filePath 完整的文件路径 "./plugins/demo.dll"
     *
     * @return int
     *  @retval 0 程序执行成功
     *  @retval -1 程序执行失败
     */
    int unloadPluginFromPath(const QString &filePath);

    inline std::vector<PluginInterface *> pluginList() { return _plugins; };
    inline QHash<QString, QPluginLoader *> loaderList() { return _loaders; };

private:
    static PluginsManager *_instance;

    std::vector<PluginInterface *> _plugins;  //插件
    QHash<QString, QPluginLoader *> _loaders; //插件路径--QPluginLoader实例
};

#endif // PLUGINSMANAGER_H
