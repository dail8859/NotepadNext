/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "GeminiCredentialStore.h"

#include "ApplicationSettings.h"

#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <QMetaType>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dpapi.h>
#endif

#ifdef Q_OS_MACOS
#include <Security/Security.h>
#endif

#ifdef Q_OS_LINUX
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDBusUnixFileDescriptor>
#include <QDBusVariant>
#include <QMap>
#include <QVariantMap>
#endif

#ifdef Q_OS_LINUX
namespace GeminiInternal {
struct SecretValue
{
    QDBusObjectPath session;
    QByteArray parameters;
    QByteArray value;
    QString contentType;
};

using SecretAttributes = QMap<QString, QString>;
using SecretMap = QMap<QDBusObjectPath, SecretValue>;
}

Q_DECLARE_METATYPE(GeminiInternal::SecretValue)
Q_DECLARE_METATYPE(GeminiInternal::SecretAttributes)
Q_DECLARE_METATYPE(GeminiInternal::SecretMap)
#endif

namespace GeminiInternal {

const char GeminiApiKeySetting[] = "Gemini/ApiKeyProtected";
const char GeminiServiceName[] = "NotepadNext Gemini";
const char GeminiAccountName[] = "Gemini API Key";

#ifdef Q_OS_WIN
std::optional<QByteArray> protectData(const QByteArray &data, QString *errorMessage)
{
    DATA_BLOB input;
    input.pbData = reinterpret_cast<BYTE *>(const_cast<char *>(data.constData()));
    input.cbData = static_cast<DWORD>(data.size());

    DATA_BLOB output;
    if (!CryptProtectData(&input, L"NotepadNext Gemini API Key", nullptr, nullptr, nullptr, 0, &output)) {
        if (errorMessage) {
            *errorMessage = QApplication::translate("GeminiCredentialStore", "Could not protect the Gemini API key with Windows Data Protection API.");
        }
        return std::nullopt;
    }

    QByteArray protectedData(reinterpret_cast<char *>(output.pbData), static_cast<int>(output.cbData));
    LocalFree(output.pbData);
    return protectedData;
}

std::optional<QByteArray> unprotectData(const QByteArray &data, QString *errorMessage)
{
    DATA_BLOB input;
    input.pbData = reinterpret_cast<BYTE *>(const_cast<char *>(data.constData()));
    input.cbData = static_cast<DWORD>(data.size());

    DATA_BLOB output;
    if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, 0, &output)) {
        if (errorMessage) {
            *errorMessage = QApplication::translate("GeminiCredentialStore", "Could not read the Gemini API key from Windows Data Protection API.");
        }
        return std::nullopt;
    }

    QByteArray unprotectedData(reinterpret_cast<char *>(output.pbData), static_cast<int>(output.cbData));
    LocalFree(output.pbData);
    return unprotectedData;
}
#endif

#ifdef Q_OS_MACOS
QByteArray toUtf8(const QString &text)
{
    return text.toUtf8();
}

CFDataRef makeCFData(const QByteArray &bytes)
{
    return CFDataCreate(kCFAllocatorDefault, reinterpret_cast<const UInt8 *>(bytes.constData()), bytes.size());
}

CFStringRef makeCFString(const QString &text)
{
    const QByteArray utf8 = toUtf8(text);
    return CFStringCreateWithBytes(kCFAllocatorDefault, reinterpret_cast<const UInt8 *>(utf8.constData()), utf8.size(), kCFStringEncodingUTF8, false);
}

CFMutableDictionaryRef makeBaseQuery()
{
    CFMutableDictionaryRef query = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFStringRef service = makeCFString(QString::fromLatin1(GeminiServiceName));
    CFStringRef account = makeCFString(QString::fromLatin1(GeminiAccountName));

    CFDictionarySetValue(query, kSecClass, kSecClassGenericPassword);
    CFDictionarySetValue(query, kSecAttrService, service);
    CFDictionarySetValue(query, kSecAttrAccount, account);

    CFRelease(service);
    CFRelease(account);
    return query;
}
#endif

#ifdef Q_OS_LINUX
QDBusArgument &operator<<(QDBusArgument &argument, const SecretValue &secret)
{
    argument.beginStructure();
    argument << secret.session << secret.parameters << secret.value << secret.contentType;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SecretValue &secret)
{
    argument.beginStructure();
    argument >> secret.session >> secret.parameters >> secret.value >> secret.contentType;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const SecretAttributes &attributes)
{
    argument.beginMap(QMetaType::fromType<QString>(), QMetaType::fromType<QString>());
    for (auto it = attributes.cbegin(); it != attributes.cend(); ++it) {
        argument.beginMapEntry();
        argument << it.key() << it.value();
        argument.endMapEntry();
    }
    argument.endMap();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SecretAttributes &attributes)
{
    argument.beginMap();
    attributes.clear();
    while (!argument.atEnd()) {
        QString key;
        QString value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        attributes.insert(key, value);
    }
    argument.endMap();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const SecretMap &secrets)
{
    argument.beginMap(QMetaType::fromType<QDBusObjectPath>(), QMetaType::fromType<SecretValue>());
    for (auto it = secrets.cbegin(); it != secrets.cend(); ++it) {
        argument.beginMapEntry();
        argument << it.key() << it.value();
        argument.endMapEntry();
    }
    argument.endMap();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SecretMap &secrets)
{
    argument.beginMap();
    secrets.clear();
    while (!argument.atEnd()) {
        QDBusObjectPath itemPath;
        SecretValue secret;
        argument.beginMapEntry();
        argument >> itemPath >> secret;
        argument.endMapEntry();
        secrets.insert(itemPath, secret);
    }
    argument.endMap();
    return argument;
}

void registerSecretTypes()
{
    static bool registered = false;
    if (registered) {
        return;
    }

    qDBusRegisterMetaType<SecretValue>();
    qDBusRegisterMetaType<SecretAttributes>();
    qDBusRegisterMetaType<SecretMap>();
    registered = true;
}

SecretAttributes geminiAttributes()
{
    return {
        {QStringLiteral("application"), QCoreApplication::applicationName()},
        {QStringLiteral("service"), QStringLiteral("gemini")},
    };
}

QDBusInterface secretServiceInterface()
{
    return QDBusInterface(QStringLiteral("org.freedesktop.secrets"),
                          QStringLiteral("/org/freedesktop/secrets"),
                          QStringLiteral("org.freedesktop.Secret.Service"),
                          QDBusConnection::sessionBus());
}

QDBusInterface secretCollectionInterface()
{
    return QDBusInterface(QStringLiteral("org.freedesktop.secrets"),
                          QStringLiteral("/org/freedesktop/secrets/collection/default"),
                          QStringLiteral("org.freedesktop.Secret.Collection"),
                          QDBusConnection::sessionBus());
}

std::optional<QDBusObjectPath> openSecretSession(QString *errorMessage)
{
    registerSecretTypes();

    QDBusInterface service = secretServiceInterface();
    if (!service.isValid()) {
        if (errorMessage) {
            *errorMessage = QApplication::translate("GeminiCredentialStore", "Secret Service is not available on this Linux desktop session.");
        }
        return std::nullopt;
    }

    QDBusMessage reply = service.call(QStringLiteral("OpenSession"), QStringLiteral("plain"), QVariant::fromValue(QDBusVariant(QString())));
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().size() < 2) {
        if (errorMessage) {
            *errorMessage = QApplication::translate("GeminiCredentialStore", "Could not open a Secret Service session.");
        }
        return std::nullopt;
    }

    return reply.arguments().at(1).value<QDBusObjectPath>();
}

QList<QDBusObjectPath> searchGeminiItems(QString *errorMessage)
{
    registerSecretTypes();

    QDBusInterface service = secretServiceInterface();
    QDBusMessage reply = service.call(QStringLiteral("SearchItems"), QVariant::fromValue(geminiAttributes()));
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().size() < 2) {
        if (errorMessage) {
            *errorMessage = QApplication::translate("GeminiCredentialStore", "Could not search Secret Service for the Gemini API key.");
        }
        return {};
    }

    QList<QDBusObjectPath> items;
    const auto unlocked = qdbus_cast<QList<QDBusObjectPath>>(reply.arguments().at(0));
    const auto locked = qdbus_cast<QList<QDBusObjectPath>>(reply.arguments().at(1));
    items.append(unlocked);
    items.append(locked);
    return items;
}
#endif

}

using namespace GeminiInternal;

GeminiCredentialStore::GeminiCredentialStore(ApplicationSettings *settings, QObject *parent)
    : QObject(parent)
    , settings(settings)
{
}

bool GeminiCredentialStore::hasApiKey(QString *errorMessage) const
{
#ifdef Q_OS_LINUX
    return !searchGeminiItems(errorMessage).isEmpty();
#elif defined(Q_OS_MACOS)
    CFMutableDictionaryRef query = makeBaseQuery();
    CFDictionarySetValue(query, kSecMatchLimit, kSecMatchLimitOne);

    const OSStatus status = SecItemCopyMatching(query, nullptr);
    CFRelease(query);

    if (status == errSecSuccess) {
        return true;
    }
    if (status != errSecItemNotFound && errorMessage) {
        *errorMessage = tr("Could not query the macOS Keychain for the Gemini API key.");
    }
    return false;
#elif defined(Q_OS_WIN)
    Q_UNUSED(errorMessage)
    return settings->contains(QLatin1String(GeminiApiKeySetting));
#else
    if (errorMessage) {
        *errorMessage = tr("Secure credential storage is not supported on this platform.");
    }
    return false;
#endif
}

std::optional<QString> GeminiCredentialStore::loadApiKey(QString *errorMessage) const
{
#ifdef Q_OS_WIN
    const QByteArray protectedData = QByteArray::fromBase64(settings->value(QLatin1String(GeminiApiKeySetting)).toByteArray());
    if (protectedData.isEmpty()) {
        return std::nullopt;
    }

    const auto data = unprotectData(protectedData, errorMessage);
    if (!data) {
        return std::nullopt;
    }

    return QString::fromUtf8(*data);
#elif defined(Q_OS_MACOS)
    CFMutableDictionaryRef query = makeBaseQuery();
    CFDictionarySetValue(query, kSecReturnData, kCFBooleanTrue);
    CFDictionarySetValue(query, kSecMatchLimit, kSecMatchLimitOne);

    CFTypeRef result = nullptr;
    const OSStatus status = SecItemCopyMatching(query, &result);
    CFRelease(query);

    if (status == errSecItemNotFound) {
        return std::nullopt;
    }
    if (status != errSecSuccess || !result) {
        if (errorMessage) {
            *errorMessage = tr("Could not read the Gemini API key from the macOS Keychain.");
        }
        return std::nullopt;
    }

    CFDataRef data = static_cast<CFDataRef>(result);
    const QByteArray apiKey(reinterpret_cast<const char *>(CFDataGetBytePtr(data)), CFDataGetLength(data));
    CFRelease(result);
    return QString::fromUtf8(apiKey);
#elif defined(Q_OS_LINUX)
    const auto session = openSecretSession(errorMessage);
    if (!session) {
        return std::nullopt;
    }

    const QList<QDBusObjectPath> items = searchGeminiItems(errorMessage);
    if (items.isEmpty()) {
        return std::nullopt;
    }

    QDBusInterface service = secretServiceInterface();
    QDBusMessage reply = service.call(QStringLiteral("GetSecrets"), QVariant::fromValue(items), QVariant::fromValue(*session));
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().isEmpty()) {
        if (errorMessage) {
            *errorMessage = tr("Could not read the Gemini API key from Secret Service.");
        }
        return std::nullopt;
    }

    const SecretMap secrets = qdbus_cast<SecretMap>(reply.arguments().constFirst());
    if (secrets.isEmpty()) {
        return std::nullopt;
    }

    return QString::fromUtf8(secrets.cbegin()->value);
#else
    if (errorMessage) {
        *errorMessage = tr("Secure credential storage is not supported on this platform.");
    }
    return std::nullopt;
#endif
}

bool GeminiCredentialStore::saveApiKey(const QString &apiKey, QString *errorMessage)
{
    const QByteArray apiKeyBytes = apiKey.toUtf8();

#ifdef Q_OS_WIN
    const auto protectedData = protectData(apiKeyBytes, errorMessage);
    if (!protectedData) {
        return false;
    }

    settings->setValue(QLatin1String(GeminiApiKeySetting), protectedData->toBase64());
    return true;
#elif defined(Q_OS_MACOS)
    CFMutableDictionaryRef query = makeBaseQuery();
    CFDataRef data = makeCFData(apiKeyBytes);

    CFMutableDictionaryRef attributes = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(attributes, kSecValueData, data);

    OSStatus status = SecItemUpdate(query, attributes);
    if (status == errSecItemNotFound) {
        CFDictionarySetValue(query, kSecValueData, data);
        status = SecItemAdd(query, nullptr);
    }

    CFRelease(attributes);
    CFRelease(data);
    CFRelease(query);

    if (status != errSecSuccess) {
        if (errorMessage) {
            *errorMessage = tr("Could not save the Gemini API key in the macOS Keychain.");
        }
        return false;
    }
    return true;
#elif defined(Q_OS_LINUX)
    const auto session = openSecretSession(errorMessage);
    if (!session) {
        return false;
    }

    QVariantMap properties;
    properties.insert(QStringLiteral("org.freedesktop.Secret.Item.Label"), QStringLiteral("NotepadNext Gemini API Key"));
    properties.insert(QStringLiteral("org.freedesktop.Secret.Item.Attributes"), QVariant::fromValue(geminiAttributes()));

    SecretValue secret{*session, QByteArray(), apiKeyBytes, QStringLiteral("text/plain")};

    QDBusInterface collection = secretCollectionInterface();
    QDBusMessage reply = collection.call(QStringLiteral("CreateItem"),
                                         properties,
                                         QVariant::fromValue(secret),
                                         true);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        if (errorMessage) {
            *errorMessage = tr("Could not save the Gemini API key in Secret Service.");
        }
        return false;
    }

    return true;
#else
    if (errorMessage) {
        *errorMessage = tr("Secure credential storage is not supported on this platform.");
    }
    return false;
#endif
}

bool GeminiCredentialStore::deleteApiKey(QString *errorMessage)
{
#ifdef Q_OS_WIN
    Q_UNUSED(errorMessage)
    settings->remove(QLatin1String(GeminiApiKeySetting));
    return true;
#elif defined(Q_OS_MACOS)
    CFMutableDictionaryRef query = makeBaseQuery();
    const OSStatus status = SecItemDelete(query);
    CFRelease(query);

    if (status == errSecSuccess || status == errSecItemNotFound) {
        return true;
    }

    if (errorMessage) {
        *errorMessage = tr("Could not delete the Gemini API key from the macOS Keychain.");
    }
    return false;
#elif defined(Q_OS_LINUX)
    const QList<QDBusObjectPath> items = searchGeminiItems(errorMessage);
    for (const QDBusObjectPath &item : items) {
        QDBusInterface itemInterface(QStringLiteral("org.freedesktop.secrets"),
                                     item.path(),
                                     QStringLiteral("org.freedesktop.Secret.Item"),
                                     QDBusConnection::sessionBus());
        QDBusMessage reply = itemInterface.call(QStringLiteral("Delete"));
        if (reply.type() == QDBusMessage::ErrorMessage) {
            if (errorMessage) {
                *errorMessage = tr("Could not delete the Gemini API key from Secret Service.");
            }
            return false;
        }
    }

    return true;
#else
    if (errorMessage) {
        *errorMessage = tr("Secure credential storage is not supported on this platform.");
    }
    return false;
#endif
}
