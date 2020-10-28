#ifndef LITH_H
#define LITH_H

#include "common.h"
#include "settings.h"
#include "protocol.h"

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
    PROPERTY_PTR(Settings, settings, new Settings(this))
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
    void resetData();

    void handleBufferInitialization(const Protocol::HData& hda);
    void handleFirstReceivedLine(const Protocol::HData& hda);
    void handleHotlistInitialization(const Protocol::HData& hda);
    void handleNicklistInitialization(const Protocol::HData& hda);

    void handleFetchLines(const Protocol::HData& hda);
    void handleHotlist(const Protocol::HData& hda);

    void _buffer_opened(const Protocol::HData& hda);
    void _buffer_type_changed(const Protocol::HData& hda);
    void _buffer_moved(const Protocol::HData& hda);
    void _buffer_merged(const Protocol::HData& hda);
    void _buffer_unmerged(const Protocol::HData& hda);
    void _buffer_hidden(const Protocol::HData& hda);
    void _buffer_unhidden(const Protocol::HData& hda);
    void _buffer_renamed(const Protocol::HData& hda);
    void _buffer_title_changed(const Protocol::HData& hda);
    void _buffer_localvar_added(const Protocol::HData& hda);
    void _buffer_localvar_changed(const Protocol::HData& hda);
    void _buffer_localvar_removed(const Protocol::HData& hda);
    void _buffer_closing(const Protocol::HData& hda);
    void _buffer_cleared(const Protocol::HData& hda);
    void _buffer_line_added(const Protocol::HData& hda);
    void _nicklist(const Protocol::HData& hda);
    void _nicklist_diff(const Protocol::HData& hda);

protected:
    void addBuffer(pointer_t ptr, Buffer *b);
    void removeBuffer(pointer_t ptr);
    Buffer *getBuffer(pointer_t ptr);
    void addLine(pointer_t bufPtr, pointer_t linePtr, BufferLine *line);
    BufferLine *getLine(pointer_t bufPtr, pointer_t linePtr);
    void addHotlist(pointer_t ptr, HotListItem *hotlist);

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
