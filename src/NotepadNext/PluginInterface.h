#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <memory>
#include <string>
#include <list>

#include <QtPlugin>

#include "PluginMessageData.h"

typedef void(__cdecl *PFUNC)(bool);

class ShortcutKey
{
public:
    enum Modifier
    {
        NO = 0x00,
        ALT = 0x01,
        CTRL = 0x02,
        SHIFT = 0x04,
    };
    ShortcutKey() : _modifier(NO), _key(0){};
    ShortcutKey(int key) : _modifier(NO), _key(key){};
    ShortcutKey(int modifier, int key) : _modifier(modifier), _key(key){};
    char _modifier = NO;
    char _key = 0;
};

struct FuncItem
{
    char _itemName[64] = {0};
    PFUNC _pFunc = nullptr;
    int _cmdID = 0;
    ShortcutKey _pShKey;
    bool _checkOnInit = false;
};

class PluginInterface
{
public:
    virtual ~PluginInterface() = default;
    virtual QString name() const = 0;
    virtual void setInfo() = 0;
    virtual std::list<FuncItem> getFuncsArray() { return _funcList; };

    virtual void recvMessage(PluginMessageData) = 0; //接收到来自创建管理器的消息
    virtual void sendMessage(PluginMessageData) = 0; //给插件管理器发消息

protected:
    std::list<FuncItem> _funcList;
};

Q_DECLARE_INTERFACE(PluginInterface, "org.galaxyworld.plugins.PluginInterface/1.0")

#endif // PLUGININTERFACE_H
