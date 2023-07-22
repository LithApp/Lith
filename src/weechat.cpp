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

Weechat::Weechat(BaseNetworkProxy *networkProxy, Lith *lith)
    : QObject(nullptr)
    , m_connection(new SocketHelper(this))
    , m_networkProxy(networkProxy)
    , m_lith(lith)
{
    connect(m_connection, &SocketHelper::dataReceived, m_networkProxy, &BaseNetworkProxy::onDataReceived, Qt::QueuedConnection);
    connect(m_networkProxy, &BaseNetworkProxy::dataReceived, this, &Weechat::onDataReceived, Qt::QueuedConnection);

    if (m_networkProxy->mode() == BaseNetworkProxy::Replay) {
        lith->statusSet(Lith::REPLAY);
    }
    else {
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

const Lith *Weechat::lith() const {
    return m_lith;
}

Lith *Weechat::lith() {
    return m_lith;
}

const QStringList supportedHashAlgos {
    "plain",
    "sha256",
    "sha512",
    "pbkdf2+sha256",
    "pbkdf2+sha512"
};
QByteArray Weechat::hashPassword(const QString &password, const QString &algo, const QByteArray &salt, int iterations) {
    if (algo == "plain") {
        return password.toUtf8();
    }
    else if (algo == "sha256") {
        QCryptographicHash qch(QCryptographicHash::Sha256);
        qch.addData(salt);
        qch.addData(password.toUtf8());
        return qch.result();
    }
    else if (algo == "sha512") {
        QCryptographicHash qch(QCryptographicHash::Sha512);
        qch.addData(salt);
        qch.addData(password.toUtf8());
        return qch.result();
    }
    else if (algo == "pbkdf2+sha256") {
        return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, password.toUtf8(), salt, iterations, 32);
    }
    else if (algo == "pbkdf2+sha512") {
        return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha512, password.toUtf8(), salt, iterations, 64);
    }
    return {};
}

QByteArray Weechat::randomString(int length) {
    QByteArray alphabet { "0123456789ABCDEF"};
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
    //connect(m_timeoutTimer, &QTimer::timeout, this, &Weechat::onTimeout, Qt::QueuedConnection);

    connect(m_hotlistTimer, &QTimer::timeout, this, &Weechat::requestHotlist, Qt::QueuedConnection);
    m_hotlistTimer->setInterval(10000);
    m_hotlistTimer->setSingleShot(false);

    connect(lith()->settingsGet(), &Settings::readyChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(lith()->settingsGet(), &Settings::networkSettingsChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);

    onConnectionSettingsChanged();
}

void Weechat::start() {
    if (m_networkProxy->mode() == ReplayProxy::Replay)
        return;
    m_connection->reset();
    m_restarting = false;
    lith()->statusSet(Lith::CONNECTING);
    restart();
}

void Weechat::restart() {
    if (m_networkProxy->mode() == ReplayProxy::Replay)
        return;

    if (m_reconnectTimer->isActive())
        m_reconnectTimer->stop();
    m_initializationStatus = UNINITIALIZED;
    auto host = lith()->settingsGet()->hostGet();
    auto port = lith()->settingsGet()->portGet();
    auto ssl = lith()->settingsGet()->encryptedGet();
    auto websocketEndpoint = lith()->settingsGet()->websocketsEndpointGet();
#ifndef __EMSCRIPTEN__
    if (!lith()->settingsGet()->useWebsocketsGet())
        m_connection->connectToTcpSocket(host, port, ssl);
    else // BEWARE
#endif // __EMSCRIPTEN__
        m_connection->connectToWebsocket(host, websocketEndpoint, port, ssl);
    // BEWARE OF THE ELSE ABOVE
}

void Weechat::onConnectionSettingsChanged() {
    auto host = lith()->settingsGet()->hostGet();
    auto pass = lith()->settingsGet()->passphraseGet();
    lith()->log(Logger::Network, "Connection settings have changed, will reset the socket");
    if (!host.isEmpty() && !pass.isEmpty()) {
        m_connection->reset();
        if (!m_restarting)
            QTimer::singleShot(50, this, &Weechat::start);
        m_restarting = true;
    }
}

void Weechat::onHandshakeAccepted(const StringMap &data) {
    if (!m_connection->isConnected())
        return;

    lith()->log(Logger::Protocol, "WeeChat handshake was accepted, authenticating");

    auto algo = data["password_hash_algo"];
    auto iterations = data["password_hash_iterations"].toInt();
    auto serverNonce = QByteArray::fromHex(data["nonce"].toLocal8Bit());
    auto clientNonce = QByteArray::fromHex(randomString(16));
    auto pass = lith()->settingsGet()->passphraseGet();

    auto salt = serverNonce + clientNonce;
    auto hash = hashPassword(pass, algo, salt, iterations);

    QString hashString;
    if (algo == "plain")
        hashString = "password=" + pass + ",compression=" + (lith()->settingsGet()->connectionCompressionGet() ? "zlib" : "off");
    else if (algo.startsWith("pbkdf2"))
        hashString = "password_hash=" + algo + ':' + salt.toHex() + ':' + QString("%1").arg(iterations) + ':' + hash.toHex();
    else
        hashString = "password_hash=" + algo + ':' + salt.toHex() + ':' + hash.toHex();

    m_initializationStatus = (Initialization) (m_initializationStatus | HANDSHAKE);

    m_connection->write(QString("init"), QString(), hashString);
    m_connection->write(QString("hdata"), QString("%1").arg(MessageNames::c_requestBuffers), QString("buffer:gui_buffers(*) number,name,short_name,hidden,title,local_variables"));
    m_connection->write(QString("hdata"), QString("%1").arg(MessageNames::c_requestFirstLine), QString("buffer:gui_buffers(*)/lines/last_line(-1)/data"));
    m_connection->write(QString("hdata"), QString("%1").arg(MessageNames::c_requestHotlist), QString("hotlist:gui_hotlist(*)"));
    m_connection->write(QString("sync"));
    m_connection->write(QString("nicklist"), QString("%1").arg(MessageNames::c_requestNicklist));
}

void Weechat::requestHotlist() {
    if (m_connection->isConnected()) {
        m_connection->write(QString("hdata"), QString("handleHotlist;%1").arg(m_messageOrder++), QString("hotlist:gui_hotlist(*)"));
    }
}


void Weechat::onConnected() {
    m_reconnectTimer->stop();
    m_reconnectTimer->setInterval(100);

    lith()->log(Logger::Protocol, "Connected to WeeChat, starting handshake");

    QTimer::singleShot(0, lith(), &Lith::resetData);
    lith()->networkErrorStringSet(QString());

    lith()->statusSet(Lith::CONNECTED);
    QString hashAlgos;
    for (auto &i : supportedHashAlgos) {
        if (!hashAlgos.isEmpty())
            hashAlgos.append(":");
        hashAlgos.append(i);
    }

    if (lith()->settingsGet()->handshakeAuthGet()) {
        m_connection->write(QString("handshake"), QString("%1").arg(MessageNames::c_handshake), QString("password_hash_algo=%1,compression=%2").arg(hashAlgos).arg(lith()->settingsGet()->connectionCompressionGet() ? "zlib" : "off"));
    }
    else {
        StringMap data;
        data["password_hash_algo"] = "plain";
        onHandshakeAccepted(data);
    }

    m_hotlistTimer->start();
}

void Weechat::onDisconnected() {
    lith()->statusSet(Lith::DISCONNECTED);

    lith()->log(Logger::Protocol, QString("WeeChat connection lost, will reconnect in %1ms").arg(m_reconnectTimer->interval() * 2));

    m_fetchBuffer.clear();
    m_bytesRemaining = 0;
    m_hotlistTimer->stop();

    if (m_reconnectTimer->interval() < 15000)
        m_reconnectTimer->setInterval(m_reconnectTimer->interval() * 2);
    m_reconnectTimer->start();
}

void Weechat::onDataReceived(const QByteArray &data) {
    auto dataCopy(data);
    onMessageReceived(dataCopy);
}

void Weechat::onError(const QString &message) {
    lith()->statusSet(Lith::ERROR);
    lith()->networkErrorStringSet("Connection failed: "+ message);
}

bool Weechat::input(pointer_t ptr, const QString &data) {
    // server doesn't reply to input commands directly so no message order here
    return m_connection->write(QString("input"), QString(), QString("0x%1 %2\n").arg(ptr, 0, 16).arg(data));
}

void Weechat::fetchLines(pointer_t ptr, int count) {
    const auto* cLith = lith();
    auto *buffer = cLith->getBuffer(ptr);
    if (buffer)
        lith()->log(Logger::Protocol, buffer->nameGet(), QString("Fetching %1 lines").arg(count));
    else
        lith()->log(Logger::Unexpected, "Attempted to fetch lines for buffer with invalid buffer");
    m_connection->write(QString("hdata"), QString("handleFetchLines;%1").arg(m_messageOrder++), QString("buffer:0x%1/lines/last_line(-%2)/data").arg(ptr, 0, 16).arg(count));
    m_timeoutTimer->start(5000);
}

void Weechat::onMessageReceived(QByteArray &data) {
    QDataStream s(&data, QIODevice::ReadOnly);

    WeeChatProtocol::String id = WeeChatProtocol::parse<WeeChatProtocol::String>(s);

    char type[4] = { 0 };
    s.readRawData(type, 3);

    if (QString(type) == "hda") {
        WeeChatProtocol::HData hda = WeeChatProtocol::parse<WeeChatProtocol::HData>(s);

        if (c_initializationMap.contains(id)) {
            // wtf, why can't I write this as |= ?
            m_initializationStatus = (Initialization) (m_initializationStatus | c_initializationMap.value(id, UNINITIALIZED));
            if (!QMetaObject::invokeMethod(Lith::instance(), id.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(WeeChatProtocol::HData, hda))) {
                lith()->log(Logger::Unexpected, QString("Possible unhandled message: %1").arg(id));
            }
        }
        else {
            auto parts = id.split(";");
            auto name = parts.first();
            if (!QMetaObject::invokeMethod(Lith::instance(), name.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(WeeChatProtocol::HData, hda))) {
                lith()->log(Logger::Unexpected, QString("Possible unhandled message: %1").arg(name));
            }
        }
    }
    else if (QString(type) == "htb") {
        WeeChatProtocol::HashTable htb = WeeChatProtocol::parse<WeeChatProtocol::HashTable>(s);

        onHandshakeAccepted(htb);
    }
    else if (QString(type) == "str") {
        WeeChatProtocol::String str = WeeChatProtocol::parse<WeeChatProtocol::String>(s);

        if (!QMetaObject::invokeMethod(Lith::instance(), id.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(const FormattedString&, str))) {
            lith()->log(Logger::Unexpected, QString("Possible unhandled message: %1").arg(id));
        }
    }
    else {
        lith()->log(Logger::Unexpected, QString("Was not able to handle message type: %1").arg(type));
    }

    if (!s.atEnd()) {
        lith()->log(Logger::Unexpected, QString("CRITICAL! Data seemingly ended but buffer is not empty"));
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
            lith()->log(Logger::Unexpected, QString("Server didn't respond to ping in time, index: %1").arg(previousPing));
            restart();
        }
        previousPing = m_messageOrder++;
        if (!m_connection->write(QString("ping"), QString("%1").arg(previousPing), QString("%1").arg(previousPing))) {
            restart();
        }
    }
    else {
        //restart();
    }
}
