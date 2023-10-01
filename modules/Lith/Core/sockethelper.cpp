#include "sockethelper.h"
#include "weechat.h"
#include "lith.h"

#include <QMetaEnum>
#include <QDataStream>

SocketHelper::SocketHelper(Weechat* parent)
    : QObject(parent) {
}

bool SocketHelper::isConnected() {
    // TODO
    if (m_webSocket && m_webSocket->isValid()) {
        return true;
    }
#ifndef __EMSCRIPTEN__
    if (m_tcpSocket && m_tcpSocket->isValid()) {
        return true;
    }
#endif
    return false;
}

Weechat* SocketHelper::weechat() {
    return qobject_cast<Weechat*>(parent());
}

Lith* SocketHelper::lith() {
    if (weechat()) {
        return weechat()->lith();
    }
    return nullptr;
}

void SocketHelper::onError(QAbstractSocket::SocketError e) {
    QMetaEnum me = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    lith()->log(
        Logger::Network, QStringLiteral("Socket error: %1 (%2)")
                             .arg((m_tcpSocket ? m_tcpSocket->errorString() : m_webSocket->errorString()))
                             .arg(QString::fromUtf8(me.valueToKey(e)))
    );

    qCritical() << Logger::Network << QStringLiteral("Socket error: %1 (%2)")
                << ((m_tcpSocket ? m_tcpSocket->errorString() : m_webSocket->errorString())) << (QString::fromUtf8(me.valueToKey(e)));
#ifndef __EMSCRIPTEN__
    if (m_tcpSocket) {
        emit errorOccurred(m_tcpSocket->errorString());
    }
#endif
    if (m_webSocket) {
        emit errorOccurred(m_webSocket->errorString());
    }
}

void SocketHelper::onDisconnected() {
    lith()->log(Logger::Network, "Socket disconnected");
    emit disconnected();
}

void SocketHelper::onConnected() {
    lith()->log(Logger::Network, "Socket connected");
    emit connected();
}

void SocketHelper::connectToWebsocket(const QString& hostname, const QString& endpoint, int port, bool encrypted) {
    reset();
    lith()->log(
        Logger::Network, QString("Connecting to: %1://%2:%3/%4").arg(encrypted ? "wss" : "ws").arg(hostname).arg(port).arg(endpoint)
    );
    m_webSocket = new QWebSocket(QStringLiteral("weechat"), QWebSocketProtocol::VersionLatest, this);

    connect(m_webSocket, &QWebSocket::connected, this, &SocketHelper::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &SocketHelper::onDisconnected);
    connect(m_webSocket, &QWebSocket::errorOccurred, this, &SocketHelper::onError);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &SocketHelper::onBinaryMessageReceived);

#ifndef __EMSCRIPTEN__
    // SSL errors need to be connected directly to handle ignoring some of them (if that's enabled)
    connect(m_webSocket, &QWebSocket::sslErrors, this, &SocketHelper::onSslErrors, Qt::DirectConnection);
#endif  // __EMSCRIPTEN__

    m_webSocket->open(
        QStringLiteral("%1://%2:%3/%4").arg(encrypted ? QStringLiteral("wss") : QStringLiteral("ws")).arg(hostname).arg(port).arg(endpoint)
    );
}

#ifndef __EMSCRIPTEN__
void SocketHelper::connectToTcpSocket(const QString& hostname, int port, bool encrypted) {
    reset();
    lith()->log(
        Logger::Network,
        QStringLiteral("Connecting to: %1:%2 (%3)").arg(hostname).arg(port).arg(encrypted ? QStringLiteral("ssl") : QStringLiteral("plain"))
    );
    m_tcpSocket = new QSslSocket(this);
    m_tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(m_tcpSocket, &QAbstractSocket::errorOccurred, this, &SocketHelper::onError, Qt::QueuedConnection);
    connect(m_tcpSocket, &QSslSocket::readyRead, this, &SocketHelper::onReadyRead, Qt::QueuedConnection);
    connect(m_tcpSocket, &QSslSocket::connected, this, &SocketHelper::onConnected, Qt::QueuedConnection);
    connect(m_tcpSocket, &QSslSocket::disconnected, this, &SocketHelper::onDisconnected, Qt::QueuedConnection);

    // SSL errors need to be connected directly to handle ignoring some of them (if that's enabled)
    connect(m_tcpSocket, &QSslSocket::sslErrors, this, &SocketHelper::onSslErrors, Qt::DirectConnection);

    if (encrypted) {
        m_tcpSocket->connectToHostEncrypted(hostname, port);
    } else {
        m_tcpSocket->connectToHost(hostname, port);
    }
}

#endif  // __EMSCRIPTEN__

qint64 SocketHelper::write(const QString& command, const QString& id, const QString& data) {
    return write(command.toUtf8(), id.toUtf8(), data.toUtf8());
}

qint64 SocketHelper::write(const QByteArray& command, const QByteArray& id, const QByteArray& data) {
    qint64 bytes = 0;
    QByteArray fullCommand;
    if (!id.isEmpty()) {
        fullCommand.append("(");
        fullCommand.append(id);
        fullCommand.append(") ");
    }
    fullCommand.append(command);
    if (!data.isEmpty()) {
        fullCommand.append(" ");
        fullCommand.append(data);
    }
    fullCommand.append("\n");
    if (command != "ping") {
        if (command == "init") {
            lith()->log(
                Logger::Protocol, QString::fromUtf8(command), QStringLiteral("Sending WeeChat command"), QStringLiteral("[obscured]")
            );
        } else {
            lith()->log(
                Logger::Protocol, QString::fromUtf8(command), QStringLiteral("Sending WeeChat command"), QStringLiteral("fullCommand")
            );
        }
    }
    if (m_webSocket) {
        bytes = m_webSocket->sendTextMessage(QString::fromUtf8(fullCommand));
    }
#ifndef __EMSCRIPTEN__
    if (m_tcpSocket) {
        bytes = m_tcpSocket->write(fullCommand);
    }
#endif  // __EMSCRIPTEN__
    if (bytes != fullCommand.size()) {
        lith()->log(
            Logger::Network,
            QStringLiteral("SocketHelper::write: Attempted to write %1 but managed to write %2 bytes").arg(fullCommand.size()).arg(bytes)
        );
    }
    return bytes;
}

void SocketHelper::reset() {
    if (m_webSocket) {
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }
#ifndef __EMSCRIPTEN__
    if (m_tcpSocket) {
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }
#endif  // __EMSCRIPTEN__
}

void SocketHelper::onBinaryMessageReceived(const QByteArray& data) {
    if (data.size() > 5) {
        auto header = data.left(5);
        if (header.size() != 5) {
            return;
        }
        qint32 bytes = 0;
        bool compressed = false;
        QDataStream s(&header, QIODevice::ReadOnly);
        s >> bytes >> compressed;
        if (bytes <= 5) {
            qCritical() << "The server sent a message header saying the message is shorter than 5 bytes, that doesn't make sense";
            m_webSocket->close();
            reset();
            return;
        }
        QByteArray dataCopy;
        if (compressed) {
            dataCopy = data.mid(1);
            dataCopy[0] = 0;
            dataCopy[1] = 0;
            dataCopy[2] = 0;
            dataCopy[3] = 0;
            dataCopy = qUncompress(dataCopy);
        } else {
            dataCopy = data.mid(5);
        }
        emit dataReceived(dataCopy);
    }
}

#ifndef __EMSCRIPTEN__
void SocketHelper::onSslErrors(const QList<QSslError>& errors) {
    QList<QSslError> ignoredSslErrors;
    for (const auto& error : errors) {
        if (Lith::settingsGet()->allowSelfSignedCertificatesGet()) {
            if (error.error() == QSslError::SelfSignedCertificate) {
                ignoredSslErrors.append(error);
            }
            if (error.error() == QSslError::SelfSignedCertificateInChain) {
                ignoredSslErrors.append(error);
            }
            if (error.error() == QSslError::HostNameMismatch) {
                ignoredSslErrors.append(error);
            }
        }
        lith()->log(Logger::Network, tr("SSL error: %1").arg(error.errorString()));
    }

    if (!ignoredSslErrors.isEmpty()) {
        if (m_tcpSocket) {
            m_tcpSocket->ignoreSslErrors(ignoredSslErrors);
        }
        if (m_webSocket) {
            m_webSocket->ignoreSslErrors(ignoredSslErrors);
        }
    }
}

void SocketHelper::onReadyRead() {
    m_timeoutTimer->stop();

    if (!m_tcpSocket) {
        // this shouldn't really happen, yet it seems it probably does
        return;
    }

    // In the protocol parser, there's the call to processEvents that could lead to
    // this slot being called while a message is already being processed
    // This is guard that prevents processing of more messages at the same moment
    static bool guard = false;
    if (guard) {
        return;
    }

    static bool compressed = false;

    // not waiting for the rest of any message, get a new header
    if (m_bytesRemaining == 0) {
        auto header = m_tcpSocket->read(5);
        if (header.size() != 5) {
            return;
        }
        QDataStream s(&header, QIODevice::ReadOnly);
        s >> m_bytesRemaining >> compressed;
        if (m_bytesRemaining <= 5) {
            qCritical() << "The server sent a message header saying the message is shorter than 5 bytes, that doesn't make sense";
            m_tcpSocket->disconnectFromHost();
            return;
        }
        m_bytesRemaining -= 5;
        m_fetchBuffer.clear();
        // add a header to the data if compressed, containing the expected length of the data
        // Qt doesn't seem to care if it's correct so just put 0 in there
        if (compressed) {
            m_fetchBuffer.append(4, 0);
        }
    }

    // continue in whatever message came before (be it from a previous readyRead or this one
    if (m_bytesRemaining > 0) {
        auto cache = m_tcpSocket->read(m_bytesRemaining);
        m_bytesRemaining -= static_cast<qint32>(cache.size());
        m_fetchBuffer.append(cache);
    }

    // one message has been received in full, process it
    if (m_bytesRemaining == 0) {
        if (compressed) {
            m_fetchBuffer = qUncompress(m_fetchBuffer);
        }
        guard = true;
        emit dataReceived(m_fetchBuffer);
        guard = false;
        m_fetchBuffer.clear();
    }

    // if there's still more data left, do one more round
    if (m_tcpSocket && m_tcpSocket->bytesAvailable()) {
        onReadyRead();
    }
}
#endif  // __EMSCRIPTEN__
