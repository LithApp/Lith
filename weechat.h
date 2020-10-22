#ifndef WEECHAT_H
#define WEECHAT_H

#include <QObject>
#include <QSslSocket>

#include <QDataStream>
#include <QMap>

#include <QQmlListProperty>
#include <QtQml>

#include <QSettings>
#include <QClipboard>

#include "common.h"
#include "datamodel.h"

class Weechat : public QObject
{
    Q_OBJECT
public:
    enum Status {
        UNCONFIGURED,
        CONNECTING,
        CONNECTED,
        DISCONNECTED,
        ERROR
    }; Q_ENUMS(Status)
    PROPERTY(Status, status)
    PROPERTY(QString, errorString)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY settingsChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY settingsChanged)
    Q_PROPERTY(bool encrypted READ encrypted WRITE setEncrypted NOTIFY settingsChanged)
    Q_PROPERTY(bool hasPassphrase READ hasPassphrase NOTIFY hasPassphraseChanged)

    Q_PROPERTY(int fetchFrom READ fetchFrom NOTIFY fetchFromChanged)
    Q_PROPERTY(int fetchTo READ fetchTo NOTIFY fetchToChanged)
public:
    static Weechat *_self;
    static Weechat *instance();

    int fetchFrom();
    int fetchTo();

private:
    explicit Weechat(QObject *parent = 0);

    QString host() const;
    int port() const;
    bool encrypted() const;
    bool hasPassphrase() const;

signals:
    void settingsChanged();
    void hasPassphraseChanged();

private slots:
    void onSettingsChanged();
    void requestHotlist();

public slots:
    void start();

    void setHost(const QString &value);
    void setPort(int value);
    void setEncrypted(bool value);
    void setPassphrase(const QString &value);

signals:
    void fetchFromChanged();
    void fetchToChanged();

public slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError e);
    void onSslErrors(const QList<QSslError> errors);
    void onMessageReceived(QByteArray &data);

    void input(pointer_t ptr, const QString &data);
    void fetchLines(pointer_t ptr, int count);

private slots:
    void onTimeout();

private:
    QSslSocket *m_connection { nullptr };

    QByteArray m_fetchBuffer;
    int32_t m_bytesRemaining { 0 };

    QString m_host { };
    int m_port { };
    QString m_passphrase { };
    bool m_useEncryption { true };

    QSettings m_settings;

    QTimer m_hotlistTimer;
    QTimer m_reconnectTimer;
    QTimer m_timeoutTimer;
};


class StuffManager : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int bufferCount READ bufferCount NOTIFY buffersChanged)
    Q_PROPERTY(Buffer* selected READ selectedBuffer NOTIFY selectedChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedChanged)
public:
    static StuffManager *instance();
    QObject *getStuff(pointer_t ptr, const QString &type = "", pointer_t parent = 0);

    int bufferCount();
    Buffer *selectedBuffer();
    int selectedIndex();
    void setSelectedIndex(int o);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void reset();

signals:
    void buffersChanged();
    void selectedChanged();
private:
    StuffManager();
    static StuffManager *_self;

    int m_selectedIndex { 0 };

    QMap<pointer_t, Buffer*> m_bufferMap;
    QList<Buffer*> m_buffers;

    QMap<pointer_t, BufferLine*> m_lineMap;
    QMap<pointer_t, HotListItem*> m_hotList;
};

#endif // WEECHAT_H
