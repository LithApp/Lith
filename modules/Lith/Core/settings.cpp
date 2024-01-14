// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
// Copyright (C) 2020 Jakub Mach
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

#include "settings.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QTimer>
#include <QCoro/QCoroThread>
#include <QCoro/QCoroSignal>

#ifndef Q_OS_WASM
  #include <QThread>
#endif  // Q_OS_WASM

#if LITH_FEATURE_KEYCHAIN
  // The path is broken with qtkeychain installed through CPM, try both variants to deal with that
  #if __has_include(<keychain.h>)
    #include <keychain.h>
  #else
    #include <qt6keychain/keychain.h>
  #endif

  #define CREDENTIAL_SERVICE_NAME "cz.rtinbriza.ma.Lith"
#endif

#include "lith.h"

Settings::Settings(QObject* parent)
    : QObject(parent) {
    // Code taken from qt-webassembly-examples

    connect(this, &Settings::readyChanged, this, &Settings::migrate, Qt::QueuedConnection);

    // m_settings will be ready at some later point in time - when
    // QSettings::status() returns NoError. (apologies for the somewhat
    // convoluted std::function and lambda use below)
    auto settingsReady = [this]() {
        const auto* mo = Settings::metaObject();
        for (int i = 0; i < mo->propertyCount(); i++) {
            auto property = mo->property(i);
            const auto* name = property.name();
            auto defaultValue = property.read(this);
            auto settingsValue = m_settings.value(name, defaultValue);
            if (defaultValue != settingsValue) {
                property.write(this, settingsValue);
            }
        }
        m_ready = true;
        emit readyChanged();
    };
    auto* testSettingsReady = new std::function<void(void)>();
    *testSettingsReady = [this, testSettingsReady, settingsReady]() {
        if (m_settings.status() == QSettings::NoError) {
            settingsReady();
            delete testSettingsReady;
        } else {
            QTimer::singleShot(10, this, *testSettingsReady);
        }
    };
    (*testSettingsReady)();
}

QCoro::Task<void> Settings::migrate() {
    co_await migrateCredentialEncryption();
    // When migrating another variable, it may be possible to come up with a
    // smarter way to do this. I think checking that m_settings contains something
    // that's not present in the metaobject should be enough
    if (!hotlistShowUnreadCountGet()) {
        if (hotlistFormatGet() == c_hotlistDefaultFormat) {
            m_settings.remove("hotlistShowUnreadCount");
            m_settings.sync();
            hotlistFormatSet({"%1", "number"});
        }
    }
    co_return;
}

void Settings::saveNetworkSettings(
    QString host, int port, bool encrypted, bool allowSelfSignedCertificates, QString passphrase, bool useEmptyPassphrase,
    bool handshakeAuth, bool connectionCompression, bool useWebsockets, QString websocketsEndpoint
) {
    bool changed = false;

    if (hostSet(host)) {
        changed = true;
    }
    if (portSet(port)) {
        changed = true;
    }
    if (encryptedSet(encrypted)) {
        changed = true;
    }
    if (allowSelfSignedCertificatesSet(allowSelfSignedCertificates)) {
        changed = true;
    }
    if (useEmptyPassphrase) {
        // Empty strings coming from QML are actually null, we differentiate between null and empty for empty passwords
        passphrase = QString("");
        passphraseSet(passphrase);
    } else if (useEmptyPassphraseGet() && passphrase.isEmpty()) {
        passphraseUnset();
    } else if (!passphrase.isEmpty()) {
        passphraseSet(passphrase);
    }
    if (handshakeAuthSet(handshakeAuth)) {
        changed = true;
    }
    if (connectionCompressionSet(connectionCompression)) {
        changed = true;
    }
#ifndef __EMSCRIPTEN__
    if (useWebsocketsSet(useWebsockets)) {
        changed = true;
    }
#endif
    if (websocketsEndpointSet(websocketsEndpoint)) {
        changed = true;
    }

    if (changed) {
        emit networkSettingsChanged();
    }
}

bool Settings::isReady() const {
    return m_ready;
}

Settings* Settings::instance() {
    static Settings self;
    return &self;
}

QCoro::Task<> Settings::migrateCredentialEncryption() {
    if (!isReady()) {
        co_return;
    }
    if (!credentialEncryptionAvailable()) {
        encryptedCredentialsSet(false);
    }

#if LITH_FEATURE_KEYCHAIN
    connect(this, &Settings::encryptedCredentialsChanged, this, &Settings::migrateCredentialEncryption);

    // TODO rewrite this mess
    if (encryptedCredentialsGet()) {
        if (m_settings.contains("host")) {
            m_settings.remove("host");
            hostSet(m_host, true);
        } else {
            m_host = (co_await getSecureValue("host", false)).value_or(QString());
            emit hostChanged();
        }

        if (m_settings.contains("port")) {
            m_settings.remove("port");
            portSet(m_port, true);
        } else {
            auto portString = co_await getSecureValue("port", false);
            if (portString.has_value()) {
                bool ok = false;
                int storedPort = portString->toInt(&ok);
                if (ok) {
                    m_port = storedPort;
                    emit portChanged();
                }
            }
        }

        if (m_settings.contains("websocketsEndpoint")) {
            m_settings.remove("websocketsEndpoint");
            websocketsEndpointSet(m_websocketsEndpoint, true);
        } else {
            auto storedEndpoint = co_await getSecureValue("websocketsEndpoint", false);
            if (storedEndpoint.has_value() && !storedEndpoint->isEmpty()) {
                m_websocketsEndpoint = storedEndpoint.value();
                emit websocketsEndpointChanged();
            }
        }

        if (m_settings.contains("passphrase")) {
            auto passphrase = m_settings.value("passphrase").toString();
            m_settings.remove("passphrase");
            passphraseSet(passphrase);
        } else {
            auto storedPassphrase = co_await getSecureValue("passphrase", false);
            if (storedPassphrase.has_value() && !storedPassphrase->isNull()) {
                hasPassphraseSet(true);
                useEmptyPassphraseSet(storedPassphrase->isEmpty());
            }
        }

        emit networkSettingsChanged();
    } else if (credentialEncryptionAvailable()) {
        auto encryptedHost = co_await getSecureValue("host", false);
        if (encryptedHost.has_value()) {
            m_host = encryptedHost.value();
            m_settings.setValue("host", m_host);
            emit hostChanged();
            co_await deleteSecureValue("host", false);
        }

        auto encryptedPort = co_await getSecureValue("port", false);
        if (encryptedPort.has_value()) {
            bool ok = false;
            int storedPort = encryptedPort->toInt(&ok);
            if (ok) {
                m_port = storedPort;
                m_settings.setValue("port", m_port);
                emit portChanged();
            }
            co_await deleteSecureValue("port", false);
        }

        auto encryptedEndpoint = co_await getSecureValue("websocketsEndpoint", false);
        if (encryptedEndpoint.has_value()) {
            m_websocketsEndpoint = encryptedEndpoint.value();
            m_settings.setValue("websocketsEndpoint", m_websocketsEndpoint);
            emit websocketsEndpointChanged();
            co_await deleteSecureValue("websocketsEndpoint", false);
        }

        auto encryptedPassphrase = co_await getSecureValue("passphrase", false);
        if (encryptedPassphrase.has_value()) {
            m_settings.setValue("passphrase", encryptedPassphrase.value());
            hasPassphraseSet(true);
            co_await deleteSecureValue("passphrase", false);
        } else {
            auto plainPassphrase = getPlainValue("passphrase");
            if (plainPassphrase.has_value() && !plainPassphrase.value().isNull()) {
                hasPassphraseSet(true);
                useEmptyPassphraseSet(plainPassphrase.value().isEmpty());
            }
        }

        emit networkSettingsChanged();
    }
    m_settings.sync();
#endif  // LITH_FEATURE_KEYCHAIN
    if (!credentialEncryptionAvailable()) {
        auto plainPassphrase = getPlainValue("passphrase");
        if (plainPassphrase.has_value() && !plainPassphrase->isNull()) {
            hasPassphraseSet(true);
            useEmptyPassphraseSet(plainPassphrase.value().isEmpty());
        }
        emit networkSettingsChanged();
    }
    co_return;
}

bool Settings::credentialEncryptionAvailable() {
#if LITH_FEATURE_KEYCHAIN
    return QKeychain::isAvailable();
#else
    return false;
#endif
}

bool Settings::hostSet(QString newHost, bool force) {
    if (!force && m_host == newHost) {
        return false;
    }

    m_host = newHost;
    emit hostChanged();
    setSensitiveValue("host", newHost).then([]() {});

    return true;
}

bool Settings::portSet(int newPort, bool force) {
    if (!force && m_port == newPort) {
        return false;
    }

    m_port = newPort;
    emit portChanged();
    auto portData = QString::number(newPort);
    setSensitiveValue("port", portData).then([]() {});
    return true;
}

void Settings::passphraseSet(QString passphrase) {
    // intentionally not stored
    getSensitiveValue("passphrase").then([this, passphrase](std::optional<QString> current) {
        setSensitiveValue("passphrase", passphrase).then([this, passphrase, current](bool success) {
            if (!success) {
                return;
            }
            if (!current.has_value() || passphrase.isEmpty() || current.value() != passphrase) {
                useEmptyPassphraseSet(passphrase.isEmpty());
                hasPassphraseSet(true);
                emit networkSettingsChanged();
            }
        });
    });
}

void Settings::useEmptyPassphraseSet(bool useEmptyPassphrase) {
    if (m_useEmptyPassphrase == useEmptyPassphrase) {
        return;
    }
    m_useEmptyPassphrase = useEmptyPassphrase;
    emit useEmptyPassphraseChanged();
}

bool Settings::websocketsEndpointSet(QString newWebsocketsEndpoint, bool force) {
    if (newWebsocketsEndpoint.isEmpty()) {
        Lith::instance()->log(Logger::Unexpected, "Attempted to set an empty websocket endpoint, replacing with \"/\"");
        newWebsocketsEndpoint = QStringLiteral("/");
    }
    if (!force && m_websocketsEndpoint == newWebsocketsEndpoint) {
        return false;
    }

    m_websocketsEndpoint = newWebsocketsEndpoint;
    emit websocketsEndpointChanged();
    setSensitiveValue("websocketsEndpoint", newWebsocketsEndpoint).then([]() {});
    return true;
}

QCoro::Task<std::optional<QString>> Settings::passphraseGet() {
    // intentionally not stored
    co_return co_await getSensitiveValue("passphrase");
}

void Settings::passphraseUnset() {
    deleteSensitiveValue("passphrase").then([this]() {
        qDebug() << "Done";
        hasPassphraseSet(false);
        useEmptyPassphraseSet(false);
        emit networkSettingsChanged();
    });
}

// TODO rewrite this mess too
QCoro::Task<std::optional<QString>> Settings::getSecureValue(QString key, bool fallback) {
#if LITH_FEATURE_KEYCHAIN
    auto* job = new QKeychain::ReadPasswordJob(CREDENTIAL_SERVICE_NAME, this);
    job->setKey(key);
    job->setAutoDelete(false);
    auto task = qCoro(job, &QKeychain::Job::finished);
    job->start();
    co_await task;
    if (job->error()) {
        if (job->error() != QKeychain::EntryNotFound) {
            Lith::instance()->log(
                Logger::Unexpected, QStringLiteral("keychain"), QStringLiteral("Secure value %1 retrieval failed").arg(key),
                job->errorString()
            );
        }
        if (fallback) {
            co_return getPlainValue(key);
        }
        co_return std::nullopt;
    }
    job->deleteLater();
    co_return job->textData();
#else
    co_return std::nullopt;
#endif
}

QCoro::Task<bool> Settings::setSecureValue(QString key, QString value, bool fallback) {
#if LITH_FEATURE_KEYCHAIN
    auto* job = new QKeychain::WritePasswordJob(CREDENTIAL_SERVICE_NAME, this);
    job->setKey(key);
    job->setAutoDelete(false);
    job->setTextData(value);
    auto task = qCoro(job, &QKeychain::Job::finished);
    job->start();
    co_await task;

    if (job->error()) {
        if (job->error() != QKeychain::EntryNotFound) {
            Lith::instance()->log(
                Logger::Unexpected, QStringLiteral("keychain"), QStringLiteral("Secure value %1 store failed").arg(key), job->errorString()
            );
        }
        if (fallback) {
            co_return setPlainValue(key, value);
        }
        co_return false;
    }
    job->deleteLater();
    co_return true;
#else
    co_return false;
#endif
}

std::optional<QString> Settings::getPlainValue(QString key) {
    if (m_settings.contains(key)) {
        auto value = m_settings.value(key).toString();
        return value;
    }
    return std::nullopt;
}

bool Settings::setPlainValue(QString key, QString value) {
    if (getPlainValue(key) != value) {
        m_settings.setValue(key, value);
        m_settings.sync();
        return true;
    }
    return false;
}

QCoro::Task<bool> Settings::deleteSecureValue(QString key, bool fallback) {
#if LITH_FEATURE_KEYCHAIN
    auto* job = new QKeychain::DeletePasswordJob(CREDENTIAL_SERVICE_NAME, this);
    job->setKey(key);
    job->setAutoDelete(false);
    auto task = qCoro(job, &QKeychain::Job::finished);
    job->start();
    co_await task;

    if (job->error()) {
        if (job->error() != QKeychain::EntryNotFound) {
            Lith::instance()->log(
                Logger::Unexpected, QStringLiteral("keychain"), QStringLiteral("Secure value %1 deletion failed").arg(key),
                job->errorString()
            );
        }
        if (fallback) {
            co_return deletePlainValue(key);
        }
        co_return false;
    }
    job->deleteLater();
    co_return true;
#else
    co_return false;
#endif
}
bool Settings::deletePlainValue(QString key) {
    if (m_settings.contains(key)) {
        m_settings.remove(key);
        m_settings.sync();
        return true;
    }
    return false;
}

QCoro::Task<bool> Settings::setSensitiveValue(QString key, QString value) {
    if (!isReady()) {
        co_return false;
    }
    if (m_encryptedCredentials) {
#ifndef Q_OS_WASM
        QThread* callerThread = QThread::currentThread();
        QThread* settingsThread = Settings::instance()->thread();
        if (callerThread != settingsThread) {
            co_await QCoro::moveToThread(settingsThread);
        }
#endif  // Q_OS_WASM
        auto result = co_await setSecureValue(key, value, false);
#ifndef Q_OS_WASM
        co_await QCoro::moveToThread(callerThread);
#endif  // Q_OS_WASM
        co_return result;
    }
    co_return setPlainValue(key, value);
}

QCoro::Task<std::optional<QString>> Settings::getSensitiveValue(const QString& key) {
    if (m_encryptedCredentials) {
#ifndef Q_OS_WASM
        QThread* callerThread = QThread::currentThread();
        QThread* settingsThread = Settings::instance()->thread();
        if (callerThread != settingsThread) {
            co_await QCoro::moveToThread(settingsThread);
        }
#endif  // Q_OS_WASM
        auto result = co_await getSecureValue(key, false);
#ifndef Q_OS_WASM
        co_await QCoro::moveToThread(callerThread);
#endif  // Q_OS_WASM
        co_return result;
    }
    co_return getPlainValue(key);
}

QCoro::Task<bool> Settings::deleteSensitiveValue(QString key) {
    if (m_encryptedCredentials) {
#ifndef Q_OS_WASM
        QThread* callerThread = QThread::currentThread();
        QThread* settingsThread = Settings::instance()->thread();
        if (callerThread != settingsThread) {
            co_await QCoro::moveToThread(settingsThread);
        }
#endif  // Q_OS_WASM
        auto result = co_await deleteSecureValue(key, false);
#ifndef Q_OS_WASM
        co_await QCoro::moveToThread(callerThread);
#endif  // Q_OS_WASM
        co_return result;
    }
    co_return deletePlainValue(key);
}
