#ifndef LITH_H
#define LITH_H

#include "common.h"
#include "settings.h"

#include <QSortFilterProxyModel>
#include <QPointer>

class Weechat;
class ProxyBufferList;

class Buffer;
class BufferLine;
class HotListItem;

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

class ProxyBufferList : public QSortFilterProxyModel {
    Q_OBJECT
    PROPERTY(QString, filterWord)
public:
    ProxyBufferList(QObject *parent = nullptr, QAbstractListModel *parentModel = nullptr);

    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};


#endif // LITH_H
