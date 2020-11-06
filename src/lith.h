// Lith
// Copyright (C) 2020 Martin Bříza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

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
public:
    enum Status {
        UNCONFIGURED,
        CONNECTING,
        CONNECTED,
        DISCONNECTED,
        ERROR
    };
    Q_ENUMS(Status)
private:
    PROPERTY(Status, status, UNCONFIGURED)
    PROPERTY(QString, errorString)
    PROPERTY_PTR(Settings, settings, new Settings(this))
    Q_PROPERTY(bool hasPassphrase READ hasPassphrase NOTIFY hasPassphraseChanged)
    //Q_PROPERTY(Weechat* weechat READ weechat CONSTANT)

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

    void handleBufferInitialization(Protocol::HData *hda);
    void handleFirstReceivedLine(Protocol::HData *hda);
    void handleHotlistInitialization(Protocol::HData *hda);
    void handleNicklistInitialization(Protocol::HData *hda);

    void handleFetchLines(Protocol::HData *hda);
    void handleHotlist(Protocol::HData *hda);

    void _buffer_opened(Protocol::HData *hda);
    void _buffer_type_changed(Protocol::HData *hda);
    void _buffer_moved(Protocol::HData *hda);
    void _buffer_merged(Protocol::HData *hda);
    void _buffer_unmerged(Protocol::HData *hda);
    void _buffer_hidden(Protocol::HData *hda);
    void _buffer_unhidden(Protocol::HData *hda);
    void _buffer_renamed(Protocol::HData *hda);
    void _buffer_title_changed(Protocol::HData *hda);
    void _buffer_localvar_added(Protocol::HData *hda);
    void _buffer_localvar_changed(Protocol::HData *hda);
    void _buffer_localvar_removed(Protocol::HData *hda);
    void _buffer_closing(Protocol::HData *hda);
    void _buffer_cleared(Protocol::HData *hda);
    void _buffer_line_added(Protocol::HData *hda);
    void _nicklist(Protocol::HData *hda);
    void _nicklist_diff(Protocol::HData *hda);

protected:
    void addBuffer(pointer_t ptr, Buffer *b);
    void removeBuffer(pointer_t ptr);
    Buffer *getBuffer(pointer_t ptr);
    void addLine(pointer_t bufPtr, pointer_t linePtr, BufferLine *line);
    BufferLine *getLine(pointer_t bufPtr, pointer_t linePtr);
    void addHotlist(pointer_t ptr, HotListItem *hotlist);
    HotListItem *getHotlist(pointer_t ptr);

signals:
    void hasPassphraseChanged();
    void selectedBufferChanged();

private:
    explicit Lith(QObject *parent = 0);

    QThread *m_weechatThread { nullptr };
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
