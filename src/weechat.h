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
class Weechat;

class Lith : public QObject
{
    Q_OBJECT
public:
    PROPERTY(QString, errorString)
    PROPERTY(Settings*, settings, new Settings(this))
    Q_PROPERTY(bool hasPassphrase READ hasPassphrase NOTIFY hasPassphraseChanged)
    Q_PROPERTY(ProxyBufferList* buffers READ buffers CONSTANT)
    Q_PROPERTY(Weechat* weechat READ weechat CONSTANT)

public:
    static Lith *_self;
    static Lith *instance();

    bool hasPassphrase() const;
    ProxyBufferList *buffers();
    Weechat *weechat();

public slots:
    void onMessageReceived(QByteArray &data);

signals:
    void hasPassphraseChanged();

private:
    explicit Lith(QObject *parent = 0);

    ProxyBufferList *m_proxyBufferList { nullptr };
    Weechat *m_weechat { nullptr };
};

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
