#include "sockethelper.h"
#include "weechat.h"
#include "lith.h"

#include <QMetaEnum>
#include <QDataStream>

SocketHelper::SocketHelper(Weechat *parent)
    : QObject(parent)
{
}

bool SocketHelper::isConnected() {
    // TODO
    if (m_webSocket && m_webSocket->isValid())
        return true;
#ifndef __EMSCRIPTEN__
    if (m_tcpSocket && m_tcpSocket->isValid())
        return true;
#endif
    return false;
}

Weechat *SocketHelper::weechat()
{
    return qobject_cast<Weechat*>(parent());
}

Lith *SocketHelper::lith()
{
    if (weechat())
        return weechat()->lith();
    return nullptr;
}

void SocketHelper::onError(QAbstractSocket::SocketError e) {
    QMetaEnum me = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    lith()->log(Logger::Network, QString("Socket error: %1").arg(me.valueToKey(e)));
#ifndef __EMSCRIPTEN__
    if (m_tcpSocket)
        emit errorOccurred(m_tcpSocket->errorString());
#endif
    if (m_webSocket)
        emit errorOccurred(m_webSocket->errorString());
}

void SocketHelper::onDisconnected() {
    lith()->log(Logger::Network, "Socket disconnected");
    emit disconnected();
}

void SocketHelper::onConnected() {
    lith()->log(Logger::Network, "Socket connected");
    emit connected();
}

void SocketHelper::connectToWebsocket(const QString &hostname, const QString &endpoint, int port, bool encrypted) {
    reset();
    lith()->log(Logger::Network, QString("Connecting to: %1://%2:%3/%4").arg(encrypted ? "wss" : "ws").arg(hostname).arg(port).arg(endpoint));
    m_webSocket = new QWebSocket("weechat", QWebSocketProtocol::VersionLatest, this);

    connect(m_webSocket, &QWebSocket::connected, this, &SocketHelper::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &SocketHelper::onDisconnected);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &SocketHelper::onError);

    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &SocketHelper::onBinaryMessageReceived);

    QList<QSslError> expectedSslErrors;
    if (weechat()->lith()->settingsGet()->allowSelfSignedCertificatesGet()) {
#ifndef __EMSCRIPTEN__
        expectedSslErrors.append(QSslError(QSslError::SelfSignedCertificate));
        expectedSslErrors.append(QSslError(QSslError::SelfSignedCertificateInChain));
#endif // __EMSCRIPTEN__
    }
#ifndef __EMSCRIPTEN__
    m_webSocket->ignoreSslErrors(expectedSslErrors);
#endif

    m_webSocket->open(QString("%1://%2:%3/%4").arg(encrypted ? "wss" : "ws").arg(hostname).arg(port).arg(endpoint));
}

#ifndef __EMSCRIPTEN__
void SocketHelper::connectToTcpSocket(const QString &hostname, int port, bool encrypted) {
    reset();
    lith()->log(Logger::Network, QString("Connecting to: %1:%2 (%3)").arg(hostname).arg(port).arg(encrypted ? "ssl" : "plain"));
    m_tcpSocket = new QSslSocket(this);
    m_tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    QList<QSslError> expectedSslErrors;
    if (weechat()->lith()->settingsGet()->allowSelfSignedCertificatesGet()) {
        expectedSslErrors.append(QSslError(QSslError::SelfSignedCertificate));
        expectedSslErrors.append(QSslError(QSslError::SelfSignedCertificateInChain));
    }
    m_tcpSocket->ignoreSslErrors(expectedSslErrors);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_tcpSocket, static_cast<void(QSslSocket::*)(QSslSocket::SocketError)>(&QAbstractSocket::errorOccurred), this, &SocketHelper::onError, Qt::QueuedConnection);
#else
    connect(m_tcpSocket, static_cast<void(QSslSocket::*)(QSslSocket::SocketError)>(&QAbstractSocket::error), this, &SocketHelper::onError, Qt::QueuedConnection);
#endif
    connect(m_tcpSocket, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), this, &SocketHelper::onSslErrors, Qt::QueuedConnection);
    connect(m_tcpSocket, &QSslSocket::readyRead, this, &SocketHelper::onReadyRead, Qt::QueuedConnection);
    connect(m_tcpSocket, &QSslSocket::connected, this, &SocketHelper::onConnected, Qt::QueuedConnection);
    connect(m_tcpSocket, &QSslSocket::disconnected, this, &SocketHelper::onDisconnected, Qt::QueuedConnection);

    if (encrypted)
        m_tcpSocket->connectToHostEncrypted(hostname, port);
    else
        m_tcpSocket->connectToHost(hostname, port);
}

#endif // __EMSCRIPTEN__

qint64 SocketHelper::write(const char *data) {
     return write(QByteArray(data));
}

qint64 SocketHelper::write(const QString &data) {
    return write(data.toUtf8());
}

qint64 SocketHelper::write(const QByteArray &data) {
    qint64 bytes = 0;
    lith()->log(Logger::Protocol, QString(), QString("Sending WeeChat command"), QString(data));
    if (m_webSocket) {
        bytes = m_webSocket->sendTextMessage(data);
    }
#ifndef __EMSCRIPTEN__
    if (m_tcpSocket) {
        bytes = m_tcpSocket->write(data);
    }
#endif // __EMSCRIPTEN__
    if (bytes != data.size()) {
        lith()->log(Logger::Network, QString("SocketHelper::write: Attempted to write %1 but managed to write %2 bytes").arg(data.size()).arg(bytes));
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
#endif // __EMSCRIPTEN__
}

void SocketHelper::onBinaryMessageReceived(const QByteArray &data) {
    if (data.size() > 5) {
        auto header = data.left(5);
        if (header.size() != 5) {
            return;
        }
        qint32 bytes;
        bool compressed;
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
        }
        else {
            dataCopy = data.mid(5);
        }
        emit dataReceived(dataCopy);
    }
}

#ifndef __EMSCRIPTEN__
void SocketHelper::onSslErrors(const QList<QSslError> &errors) {
    for (auto& i : errors) {
        lith()->log(Logger::Network, "SSL error: " + i.errorString());
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
    if (guard)
        return;

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
        if (compressed)
            m_fetchBuffer.append(4, 0);
    }

    // continue in whatever message came before (be it from a previous readyRead or this one
    if (m_bytesRemaining > 0) {
        auto cache = m_tcpSocket->read(m_bytesRemaining);
        m_bytesRemaining -= cache.size();
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
#endif // __EMSCRIPTEN__
