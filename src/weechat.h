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

#include "qmlobjectlist.h"

class ProxyBufferList;
class Weechat;

class Lith : public QObject {
    Q_OBJECT
    PROPERTY(QString, errorString)
    PROPERTY(Settings*, settings, new Settings(this))
    Q_PROPERTY(bool hasPassphrase READ hasPassphrase NOTIFY hasPassphraseChanged)
    Q_PROPERTY(Weechat* weechat READ weechat CONSTANT)

    Q_PROPERTY(ProxyBufferList* buffers READ buffers CONSTANT)
    Q_PROPERTY(QmlObjectList* unfilteredBuffers READ unfilteredBuffers CONSTANT)
    Q_PROPERTY(Buffer* selectedBuffer READ selectedBuffer WRITE selectedBufferSet NOTIFY selectedBufferChanged)
    Q_PROPERTY(int selectedBufferIndex READ selectedBufferIndex WRITE selectedBufferIndexSet NOTIFY selectedBufferChanged)

public:
    static Lith *_self;
    static Lith *instance();

    bool hasPassphrase() const;
    Weechat *weechat();

    ProxyBufferList *buffers();
    QmlObjectList *unfilteredBuffers();
    Buffer *selectedBuffer();
    void selectedBufferSet(Buffer *b);
    int selectedBufferIndex();
    void selectedBufferIndexSet(int index);

    QObject *getObject(pointer_t ptr, const QString &type, pointer_t parent = 0);

public slots:
    void onMessageReceived(QByteArray &data);

    void resetData();

signals:
    void hasPassphraseChanged();
    void selectedBufferChanged();

private:
    explicit Lith(QObject *parent = 0);

    Weechat *m_weechat { nullptr };
    QmlObjectList *m_buffers { nullptr };
    ProxyBufferList *m_proxyBufferList { nullptr };
    int m_selectedBufferIndex { -1 };

    QMap<pointer_t, QPointer<Buffer>> m_bufferMap {};
    QMap<pointer_t, QPointer<BufferLine>> m_lineMap;
    QMap<pointer_t, QPointer<HotListItem>> m_hotList;
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
    ProxyBufferList(QObject *parent = nullptr, QAbstractListModel *parentModel = nullptr);

    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // WEECHAT_H
