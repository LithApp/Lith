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
#include "settings.h"

class ProxyBufferList;

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
    Q_PROPERTY(ProxyBufferList* buffers READ buffers CONSTANT)

public:
    static Weechat *_self;
    static Weechat *instance();

    ProxyBufferList *buffers();

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
    void restart();

    void setHost(const QString &value);
    void setPort(int value);
    void setEncrypted(bool value);
    void setPassphrase(const QString &value);

public slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError e);
    void onSslErrors(const QList<QSslError> &errors);
    void onMessageReceived(QByteArray &data);

    void input(pointer_t ptr, const QString &data);
    void fetchLines(pointer_t ptr, int count);

private slots:
    void onTimeout();

private:
    QSslSocket *m_connection { nullptr };
    ProxyBufferList *m_proxyBufferList { nullptr };

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

class ProxyBufferList : public QSortFilterProxyModel {
    Q_OBJECT
    PROPERTY(QString, filterWord)
public:
    ProxyBufferList(QObject *parent = nullptr);

    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

class StuffManager : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int bufferCount READ bufferCount NOTIFY buffersChanged)
    Q_PROPERTY(Buffer* selected READ selectedBuffer WRITE selectedBufferSet NOTIFY selectedChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedChanged)
public:
    static StuffManager *instance();
    QObject *getStuff(pointer_t ptr, const QString &type = "", pointer_t parent = 0);

    int bufferCount();
    Buffer *selectedBuffer();
    void selectedBufferSet(Buffer *b);
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

    QMap<pointer_t, QPointer<Buffer>> m_bufferMap;
    QList<QPointer<Buffer>> m_buffers;

    QMap<pointer_t, QPointer<BufferLine>> m_lineMap;
    QMap<pointer_t, QPointer<HotListItem>> m_hotList;
};

#endif // WEECHAT_H
