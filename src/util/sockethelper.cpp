#include "sockethelper.h"

#include <QDataStream>

SocketHelper::SocketHelper(QObject *parent)
    : QObject(parent)
{
}

bool SocketHelper::isConnected() {
    // TODO
    if (m_webSocket && m_webSocket->isValid())
        return true;
#ifndef Q_OS_WASM
    if (m_tcpSocket && m_tcpSocket->isValid())
        return true;
#endif
    return false;
}

void SocketHelper::onError(QAbstractSocket::SocketError e) {
    qWarning() << "Error!" << e;
#ifndef Q_OS_WASM
    if (m_tcpSocket)
        emit errorOccurred(m_tcpSocket->errorString());
#endif
    if (m_webSocket)
        emit errorOccurred(m_webSocket->errorString());
}

void SocketHelper::onDisconnected() {
    qCritical() << "Disconnected";

    emit disconnected();
}

void SocketHelper::onConnected() {
    qCritical() << "Connected";
    emit connected();
}

void SocketHelper::connectToWebsocket(const QString &hostname, int port, bool encrypted) {
    reset();
    qCritical() << "Trying to connect to:" << QString("%1://%2:%3").arg(encrypted ? "wss" : "ws").arg(hostname).arg(port);
    m_webSocket = new QWebSocket("weechat", QWebSocketProtocol::VersionLatest, this);

    connect(m_webSocket, &QWebSocket::connected, this, &SocketHelper::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &SocketHelper::onDisconnected);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &SocketHelper::onError);

    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &SocketHelper::onBinaryMessageReceived);

    m_webSocket->open(QString("%1://%2:%3/weechat").arg(encrypted ? "wss" : "ws").arg(hostname).arg(port));
}

#ifndef Q_OS_WASM
void SocketHelper::connectToTcpSocket(const QString &hostname, int port, bool encrypted) {
    reset();
    m_tcpSocket = new QSslSocket(this);
    m_tcpSocket->ignoreSslErrors({QSslError::UnableToGetLocalIssuerCertificate});
    m_tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

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

#endif // Q_OS_WASM

qint64 SocketHelper::write(const char *data) {
     return write(QByteArray(data));
}

qint64 SocketHelper::write(const QString &data) {
    return write(data.toUtf8());
}

qint64 SocketHelper::write(const QByteArray &data) {
    qint64 bytes = 0;
    if (m_webSocket) {
        bytes = m_webSocket->sendTextMessage(data);
    }
#ifndef Q_OS_WASM
    if (m_tcpSocket) {
        bytes = m_tcpSocket->write(data);
    }
#endif // Q_OS_WASM
    if (bytes != data.count()) {
        qWarning() << "fetchLines: Attempted to write" << data.count() << "but managed to write" << bytes;
    }
    return bytes;
}

void SocketHelper::reset() {
    if (m_webSocket) {
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }
#ifndef Q_OS_WASM
    if (m_tcpSocket) {
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
    }
#endif // Q_OS_WASM
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

#ifndef Q_OS_WASM
void SocketHelper::onSslErrors(const QList<QSslError> &errors) {
    m_tcpSocket->ignoreSslErrors(errors);
    for (auto i : errors) {
        qWarning() << "SSL Error!" << i.errorString();
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
        m_bytesRemaining -= cache.count();
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
#endif // Q_OS_WASM
