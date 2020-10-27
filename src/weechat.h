#ifndef WEECHAT_H
#define WEECHAT_H

#include "common.h"
#include "settings.h"

#include <QSslSocket>
#include <QDataStream>
#include <QTimer>

class Lith;

class Weechat : public QObject {
public:
    enum Status {
        UNCONFIGURED,
        CONNECTING,
        CONNECTED,
        DISCONNECTED,
        ERROR
    }; Q_ENUMS(Status)

    Q_OBJECT
    PROPERTY(Status, status)
public:
    Weechat(Lith *lith = nullptr);
    Lith *lith();

public slots:
    void start();
    void restart();

    void input(pointer_t ptr, const QString &data);
    void fetchLines(pointer_t ptr, int count);

private slots:
    void requestHotlist();
    void onTimeout();

    void onConnectionSettingsChanged();

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError e);
    void onSslErrors(const QList<QSslError> &errors);

private:
    QSslSocket *m_connection { nullptr };

    QByteArray m_fetchBuffer;
    int32_t m_bytesRemaining { 0 };

    QTimer m_hotlistTimer;
    QTimer m_reconnectTimer;
    QTimer m_timeoutTimer;
};

#endif // WEECHAT_H
