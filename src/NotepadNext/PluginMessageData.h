#ifndef PLUGINMESSAGEDATA_H
#define PLUGINMESSAGEDATA_H

#include <QJsonObject>

struct PluginMessageData
{
    QString from; //消息来源
    QString dest; //消息目的地
    QString msg;

    QObject *object = nullptr;
    QJsonObject info = QJsonObject();
};

Q_DECLARE_METATYPE(PluginMessageData); //确保类型可以通过信号槽传递

#endif // PLUGINMESSAGEDATA_H
