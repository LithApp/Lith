#ifndef SOCKETHELPER_H
#define SOCKETHELPER_H

#include <QObject>
#include <QTimer>

#include <QtWebSockets/QWebSocket>
#ifndef Q_OS_WASM
#include <QSslSocket>
#endif // Q_OS_WASM

class SocketHelper : public QObject {
    Q_OBJECT
public:
    SocketHelper(QObject *parent = nullptr);

    bool isConnected();

public slots:
    void reset();
    void restart();

    void connectToWebsocket(const QString &hostname, int port, bool encrypted);
#ifndef Q_OS_WASM
    void connectToTcpSocket(const QString &hostname, int port, bool encrypted);
#endif // Q_OS_WASM

    qint64 write(const char *data);
    qint64 write(const QString &data);
    qint64 write(const QByteArray &data);

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &message);

private slots:
    void onError(QAbstractSocket::SocketError e);
    void onDisconnected();
    void onConnected();

#ifndef Q_OS_WASM
    void onReadyRead();
    void onSslErrors(const QList<QSslError> &errors);
#endif // Q_OS_WASM

    void onBinaryMessageReceived(const QByteArray &data);
private:
    QTimer *m_reconnectTimer { new QTimer(this) };
    QTimer *m_timeoutTimer { new QTimer(this) };

    QWebSocket *m_webSocket { nullptr };
#ifndef Q_OS_WASM
    QSslSocket *m_tcpSocket { nullptr };
    QByteArray m_fetchBuffer;
    qint32 m_bytesRemaining { 0 };
#endif // Q_OS_WASM
};

#endif // SOCKETHELPER_H
