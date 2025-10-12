// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
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

#include "weechat.h"

#include "lith.h"
#include "protocol.h"

#include <QThread>

#include <QPasswordDigestor>
#include <QCryptographicHash>
#include <QRandomGenerator>

namespace {
    using InitializationMap = QMap<QString, Weechat::Initialization>;
    Q_GLOBAL_STATIC(
        const InitializationMap, c_initializationMap,
        {
            {       Weechat::MessageNames::c_handshake,          Weechat::HANDSHAKE},
            {  Weechat::MessageNames::c_requestBuffers,    Weechat::REQUEST_BUFFERS},
            {Weechat::MessageNames::c_requestFirstLine, Weechat::REQUEST_FIRST_LINE},
            {  Weechat::MessageNames::c_requestHotlist,    Weechat::REQUEST_HOTLIST},
            { Weechat::MessageNames::c_requestNicklist,   Weechat::REQUEST_NICKLIST}
    }
    );
}  // namespace

Weechat::Weechat(BaseNetworkProxy* networkProxy, Lith* lith)
    : QObject(nullptr)
    , m_connection(new SocketHelper(this))
    , m_networkProxy(networkProxy)
    , m_lith(lith) {
    connect(m_connection, &SocketHelper::dataReceived, m_networkProxy, &BaseNetworkProxy::onDataReceived, Qt::QueuedConnection);
    connect(m_networkProxy, &BaseNetworkProxy::dataReceived, this, &Weechat::onDataReceived, Qt::QueuedConnection);

    if (m_networkProxy->mode() == BaseNetworkProxy::Replay) {
        lith->statusSet(Lith::REPLAY);
    } else {
        connect(m_connection, &SocketHelper::connected, this, &Weechat::onConnected, Qt::QueuedConnection);
        connect(m_connection, &SocketHelper::disconnected, this, &Weechat::onDisconnected, Qt::QueuedConnection);
        connect(m_connection, &SocketHelper::errorOccurred, this, &Weechat::onError, Qt::QueuedConnection);

        connect(lith, &Lith::pongReceived, this, &Weechat::onPongReceived, Qt::QueuedConnection);
        connect(m_pingTimer, &QTimer::timeout, this, &Weechat::onPingTimeout, Qt::QueuedConnection);
        m_pingTimer->setSingleShot(false);
        m_pingTimer->start(5000);

        connect(m_reconnectTimer, &QTimer::timeout, this, &Weechat::restart, Qt::QueuedConnection);
        m_reconnectTimer->setInterval(100);
        m_reconnectTimer->setSingleShot(false);
    }
}

const Lith* Weechat::lith() const {
    return m_lith;
}

Lith* Weechat::lith() {
    return m_lith;
}

constexpr std::array<ConstLatin1String, 5> supportedHashAlgos {
    {ConstLatin1String("plain"), ConstLatin1String("sha256"), ConstLatin1String("sha512"), ConstLatin1String("pbkdf2+sha256"),
     ConstLatin1String("pbkdf2+sha512")}
};
QByteArray Weechat::hashPassword(const QString& password, const QString& algo, const QByteArray& salt, int iterations) {
    if (algo == QStringLiteral("plain")) {
        return password.toUtf8();
    } else if (algo == QStringLiteral("sha256")) {
        QCryptographicHash qch(QCryptographicHash::Sha256);
        qch.addData(salt);
        qch.addData(password.toUtf8());
        return qch.result();
    } else if (algo == QStringLiteral("sha512")) {
        QCryptographicHash qch(QCryptographicHash::Sha512);
        qch.addData(salt);
        qch.addData(password.toUtf8());
        return qch.result();
    } else if (algo == QStringLiteral("pbkdf2+sha256")) {
        return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, password.toUtf8(), salt, iterations, 32);
    } else if (algo == QStringLiteral("pbkdf2+sha512")) {
        return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha512, password.toUtf8(), salt, iterations, 64);
    }
    return {};
}

QByteArray Weechat::randomString(int length) {
    QByteArray alphabet {"0123456789ABCDEF"};
    QByteArray result;
    auto rg = QRandomGenerator::securelySeeded();
    for (int i = 0; i < length; i++) {
        result += alphabet[rg.bounded(0, alphabet.size())];
    }
    return result;
}

void Weechat::init() {
    m_timeoutTimer->setInterval(5000);
    m_timeoutTimer->setSingleShot(true);
    // connect(m_timeoutTimer, &QTimer::timeout, this, &Weechat::onTimeout, Qt::QueuedConnection);

    connect(m_hotlistTimer, &QTimer::timeout, this, &Weechat::requestHotlist, Qt::QueuedConnection);
    m_hotlistTimer->setInterval(2500);
    m_hotlistTimer->setSingleShot(false);

    connect(Lith::settingsGet(), &Settings::readyChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(Lith::settingsGet(), &Settings::networkSettingsChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);

    QTimer::singleShot(0, this, &Weechat::onConnectionSettingsChanged);
}

void Weechat::start() {
    if (m_networkProxy->mode() == ReplayProxy::Replay) {
        return;
    }
    m_connection->reset();
    m_restarting = false;
    lith()->statusSet(Lith::CONNECTING);
    restart();
}

void Weechat::userRequestedRestart() {
    m_passwordAttempts = 0;
    m_reconnectTimer->setInterval(100);
    restart();
}

void Weechat::restart() {
    if (m_networkProxy->mode() == ReplayProxy::Replay) {
        return;
    }

    if (m_reconnectTimer->isActive()) {
        m_reconnectTimer->stop();
    }
    m_initializationStatus = UNINITIALIZED;
    auto host = Lith::settingsGet()->hostGet();
    auto port = Lith::settingsGet()->portGet();
    auto ssl = Lith::settingsGet()->encryptedGet();
    auto websocketEndpoint = Lith::settingsGet()->websocketsEndpointGet();
#ifndef __EMSCRIPTEN__
    if (!Lith::settingsGet()->useWebsocketsGet() && !host.isEmpty()) {
        m_connection->connectToTcpSocket(host, port, ssl);
    } else if (!host.isEmpty()) {
#endif  // __EMSCRIPTEN__
        m_connection->connectToWebsocket(host, websocketEndpoint, port, ssl);
#ifndef __EMSCRIPTEN__
    }
#endif  // __EMSCRIPTEN__
    // BEWARE OF THE ELSE ABOVE
}

void Weechat::onConnectionSettingsChanged() {
    m_passwordAttempts = 0;

    bool hasPassphrase = Lith::settingsGet()->hasPassphraseGet();
#ifndef __EMSCRIPTEN__
    bool usesWebsockets = Lith::settingsGet()->useWebsocketsGet();
#else
    bool usesWebsockets = true;
#endif
    bool hasHost = !Lith::settingsGet()->hostGet().isEmpty();
    bool hasWSEndpoint = !Lith::settingsGet()->websocketsEndpointGet().isEmpty();

    lith()->log(Logger::Network, QStringLiteral("Connection settings have changed, will reset the socket"));
    if (hasHost && hasPassphrase && (!usesWebsockets || hasWSEndpoint)) {
        m_connection->reset();
        if (!m_restarting) {
            QTimer::singleShot(50, this, &Weechat::start);
        }
        m_restarting = true;
    } else {
        m_connection->reset();
        m_reconnectTimer->stop();
    }
}

QCoro::Task<void> Weechat::onHandshakeAccepted(StringMap data) {
    if (!m_connection->isConnected()) {
        co_return;
    }

    lith()->log(Logger::Protocol, QStringLiteral("WeeChat handshake was accepted, authenticating"));

    auto algo = data[QStringLiteral("password_hash_algo")];
    auto iterations = data[QStringLiteral("password_hash_iterations")].toInt();
    auto serverNonce = QByteArray::fromHex(data[QStringLiteral("nonce")].toLocal8Bit());
    auto clientNonce = QByteArray::fromHex(randomString(16));
    QString pass;
    auto maybePass = co_await Lith::settingsGet()->passphraseGet();
    if (maybePass.has_value()) {
        pass = maybePass.value();
    } else {
        Lith::instance()->log(Logger::Protocol, QStringLiteral("Can't authenticate without a valid password"));
    }

    if (pass.isNull()) {
        co_return;
    }

    auto salt = serverNonce + clientNonce;
    auto hash = hashPassword(pass, algo, salt, iterations);

    QString hashString;
    if (algo == QStringLiteral("plain")) {
        hashString = QStringLiteral("password=%1,compression=%2")
                         .arg(pass)
                         .arg(Lith::settingsGet()->connectionCompressionGet() ? QStringLiteral("zlib") : QStringLiteral("off"));
    } else if (algo.startsWith(QStringLiteral("pbkdf2"))) {
        hashString = QStringLiteral("password_hash=%1:%2:%3:%4").arg(algo).arg(salt.toHex()).arg(iterations).arg(hash.toHex());
    } else {
        hashString = QStringLiteral("password_hash=%1:%2:%3").arg(algo).arg(salt.toHex()).arg(hash.toHex());
    }

    m_initializationStatus = static_cast<Initialization>(m_initializationStatus | HANDSHAKE);
    m_passwordAttempts++;

    m_connection->write(QStringLiteral("init"), QString(), hashString);
    m_connection->write(
        QStringLiteral("hdata"), QStringLiteral("%1").arg(MessageNames::c_requestBuffers),
        QStringLiteral("buffer:gui_buffers(*) number,name,short_name,hidden,title,local_variables")
    );
    m_connection->write(
        QStringLiteral("hdata"), QStringLiteral("%1").arg(MessageNames::c_requestFirstLine),
        QStringLiteral("buffer:gui_buffers(*)/own_lines/last_line(-1)/data")
    );
    m_connection->write(
        QStringLiteral("hdata"), QStringLiteral("%1").arg(MessageNames::c_requestHotlist), QStringLiteral("hotlist:gui_hotlist(*)")
    );
    m_connection->write(QStringLiteral("sync"));
    m_connection->write(QStringLiteral("nicklist"), QStringLiteral("%1").arg(MessageNames::c_requestNicklist));
}

void Weechat::requestHotlist() {
    if (m_connection->isConnected()) {
        m_connection->write(
            QStringLiteral("hdata"), QStringLiteral("handleHotlist;%1").arg(m_messageOrder++), QStringLiteral("hotlist:gui_hotlist(*)")
        );
    }
}

void Weechat::onConnected() {
    m_reconnectTimer->stop();
    m_initializationStatus = static_cast<Initialization>(m_initializationStatus | CONNECTION_OPENED);

    lith()->log(Logger::Protocol, QStringLiteral("Connected to WeeChat, starting handshake"));

    QTimer::singleShot(0, lith(), &Lith::resetData);
    lith()->networkErrorStringSet(QString());

    lith()->statusSet(Lith::CONNECTED);
    QString hashAlgos;
    for (const auto& i : supportedHashAlgos) {
        if (!hashAlgos.isEmpty()) {
            hashAlgos.append(QStringLiteral(":"));
        }
        hashAlgos.append(i);
    }

    if (Lith::settingsGet()->handshakeAuthGet()) {
        m_connection->write(
            QStringLiteral("handshake"), QStringLiteral("%1").arg(MessageNames::c_handshake),
            QStringLiteral("password_hash_algo=%1,compression=%2")
                .arg(hashAlgos)
                .arg(Lith::settingsGet()->connectionCompressionGet() ? QStringLiteral("zlib") : QStringLiteral("off"))
        );
    } else {
        StringMap data;
        data[QStringLiteral("password_hash_algo")] = QStringLiteral("plain");
        onHandshakeAccepted(data);
    }

    m_hotlistTimer->start();
}

void Weechat::onDisconnected() {
    if (Lith::settingsGet()->hasPassphraseGet()) {
        lith()->statusSet(Lith::DISCONNECTED);
    } else {
        lith()->statusSet(Lith::UNCONFIGURED);
    }

    m_fetchBuffer.clear();
    m_bytesRemaining = 0;
    m_hotlistTimer->stop();

    if (m_initializationStatus == UNTIL_HANDSHAKE) {
        lith()->log(Logger::Protocol, QStringLiteral("Authentication failed, attempt number %1.").arg(m_passwordAttempts));
        if (m_passwordAttempts == 3) {
            lith()->statusSet(Lith::ERROR);
            lith()->errorStringSet(tr("Authentication failed with remote host. Please check your login credentials"));
            m_initializationStatus = UNINITIALIZED;
            m_reconnectTimer->stop();
            m_passwordAttempts = 0;
            return;
        }
    }

    lith()->log(Logger::Protocol, QStringLiteral("WeeChat connection lost, will reconnect in %1ms").arg(m_reconnectTimer->interval() * 2));

    if (m_reconnectTimer->interval() < 15000) {
        m_reconnectTimer->setInterval(m_reconnectTimer->interval() * 2);
    }
    m_reconnectTimer->start();
}

void Weechat::onDataReceived(const QByteArray& data) {
    auto dataCopy(data);
    onMessageReceived(dataCopy);
}

void Weechat::onError(const QString& message) {
    lith()->statusSet(Lith::ERROR);
    lith()->networkErrorStringSet(QStringLiteral("Connection failed: %1").arg(message));
}

bool Weechat::input(weechat_pointer_t ptr, const QByteArray& data) {
    // server doesn't reply to input commands directly so no message order here
    return m_connection->write(
        QByteArrayLiteral("input"), QByteArray(),
        QByteArrayLiteral("0x") + QByteArray::number(ptr, 16) + QByteArrayLiteral(" ") + data + QByteArrayLiteral("\n")
    );
}

void Weechat::fetchLines(weechat_pointer_t ptr, int count) {
    const auto* cLith = lith();
    const auto* buffer = cLith->getBuffer(ptr);
    if (buffer) {
        lith()->log(Logger::Protocol, buffer->nameGet().toPlain(), QStringLiteral("Fetching %1 lines").arg(count));
    } else {
        lith()->log(Logger::Unexpected, QStringLiteral("Attempted to fetch lines for buffer with invalid buffer"));
    }
    m_connection->write(
        QStringLiteral("hdata"), QStringLiteral("handleFetchLines;%1").arg(m_messageOrder++),
        QStringLiteral("buffer:0x%1/own_lines/last_line(-%2)/data").arg(ptr, 0, 16).arg(count)
    );
    m_timeoutTimer->start(5000);
}

void Weechat::onMessageReceived(QByteArray& data) {
    QDataStream s(&data, QIODevice::ReadOnly);

    WeeChatProtocol::PlainString id = WeeChatProtocol::parse<WeeChatProtocol::PlainString>(s);

    std::array<char, 3> typeData = {0, 0, 0};
    s.readRawData(typeData.data(), 3);
    QByteArrayView type(typeData.data(), 3);

    if (type == QByteArrayLiteral("hda")) {
        WeeChatProtocol::HData hda = WeeChatProtocol::parse<WeeChatProtocol::HData>(s);

        if (c_initializationMap->contains(id)) {
            // wtf, why can't I write this as |= ?
            m_initializationStatus = static_cast<Initialization>(m_initializationStatus | c_initializationMap->value(id, UNINITIALIZED));
            if (!QMetaObject::invokeMethod(
                    Lith::instance(), id.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(WeeChatProtocol::HData, hda)
                )) {
                lith()->log(Logger::Unexpected, QStringLiteral("Possible unhandled message: %1").arg(id));
            }
            if (m_initializationStatus & COMPLETE) {
                m_passwordAttempts = 0;
                m_reconnectTimer->setInterval(100);
            }
        } else {
            auto parts = id.split(QStringLiteral(";"));
            const auto& name = parts.first();
            if (!QMetaObject::invokeMethod(
                    Lith::instance(), name.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(WeeChatProtocol::HData, hda)
                )) {
                lith()->log(Logger::Unexpected, QStringLiteral("Possible unhandled message: %1").arg(name));
            }
        }
    } else if (type == QByteArrayLiteral("htb")) {
        WeeChatProtocol::HashTable htb = WeeChatProtocol::parse<WeeChatProtocol::HashTable>(s);

        onHandshakeAccepted(htb);
    } else if (type == QByteArrayLiteral("str")) {
        WeeChatProtocol::String str = WeeChatProtocol::parse<WeeChatProtocol::String>(s);

        if (!QMetaObject::invokeMethod(
                Lith::instance(), id.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(const FormattedString&, str)
            )) {
            lith()->log(Logger::Unexpected, QStringLiteral("Possible unhandled message: %1").arg(id));
        }
    } else {
        lith()->log(Logger::Unexpected, QStringLiteral("Was not able to handle message type: %1").arg(QLatin1String(type.data())));
    }

    if (!s.atEnd()) {
        lith()->log(Logger::Unexpected, QStringLiteral("CRITICAL! Data seemingly ended but buffer is not empty"));
    }
}

void Weechat::onPongReceived(qint64 id) {
    m_lastReceivedPong = id;
}

void Weechat::onTimeout() {
    m_connection->reset();
    lith()->statusSet(Lith::DISCONNECTED);
    start();
}

void Weechat::onPingTimeout() {
    static qint64 previousPing = 0;
    if (m_initializationStatus == COMPLETE) {
        if (previousPing < m_lastReceivedPong - 1) {
            lith()->log(Logger::Unexpected, QStringLiteral("Server didn't respond to ping in time, index: %1").arg(previousPing));
            restart();
        }
        previousPing = m_messageOrder++;
        if (!m_connection->write(QStringLiteral("ping"), QStringLiteral("%1").arg(previousPing), QStringLiteral("%1").arg(previousPing))) {
            restart();
        }
    } else {
        // restart();
    }
}
