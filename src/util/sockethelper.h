#ifndef SOCKETHELPER_H
#define SOCKETHELPER_H

#include <QObject>
#include <QTimer>

#include <QtWebSockets/QWebSocket>
#ifndef __EMSCRIPTEN__
#include <QSslSocket>
#endif // __EMSCRIPTEN__
#include <QSslError>

class Weechat;
class Lith;
class SocketHelper : public QObject {
    Q_OBJECT
public:
    explicit SocketHelper(Weechat *parent = nullptr);

    bool isConnected();

    Weechat *weechat();
    Lith *lith();

public slots:
    void reset();

    void connectToWebsocket(const QString &hostname, const QString &endpoint, int port, bool encrypted);
#ifndef __EMSCRIPTEN__
    void connectToTcpSocket(const QString &hostname, int port, bool encrypted);
#endif // __EMSCRIPTEN__

    qint64 write(const QString& command, const QString& id = {}, const QString &data = {});
    qint64 write(const QByteArray& command, const QByteArray& id = {}, const QByteArray &data = {});

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &message);

private slots:
    void onError(QAbstractSocket::SocketError e);
    void onDisconnected();
    void onConnected();

#ifndef __EMSCRIPTEN__
    void onReadyRead();
    void onSslErrors(const QList<QSslError> &errors);
#endif // __EMSCRIPTEN__

    void onBinaryMessageReceived(const QByteArray &data);
private:
    QTimer *m_timeoutTimer { new QTimer(this) };

    QWebSocket *m_webSocket { nullptr };
#ifndef __EMSCRIPTEN__
    QSslSocket *m_tcpSocket { nullptr };
    QByteArray m_fetchBuffer;
    qint32 m_bytesRemaining { 0 };
#endif // __EMSCRIPTEN__
};

#endif // SOCKETHELPER_H
