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
#include "logger.h"
#include "settings.h"
#include "protocol.h"
#include "datamodel.h"
#include "windowhelper.h"
#include "search.h"
#include "util/nicklistfilter.h"
#include "util/messagelistfilter.h"

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
        ERROR,
        REPLAY
    };
    Q_ENUM(Status)
private:
    PROPERTY(Status, status, UNCONFIGURED)
    Q_PROPERTY(QString statusString READ statusString NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorStringGet WRITE errorStringSet NOTIFY errorStringChanged)
    Q_PROPERTY(Settings* settings READ settingsGet CONSTANT)
    PROPERTY_PTR(WindowHelper, windowHelper)

    Q_PROPERTY(ProxyBufferList* buffers READ buffers CONSTANT)
    Q_PROPERTY(QmlObjectList* unfilteredBuffers READ unfilteredBuffers CONSTANT)
    Q_PROPERTY(Buffer* selectedBuffer READ selectedBuffer WRITE selectedBufferSet NOTIFY selectedBufferChanged)
    Q_PROPERTY(int selectedBufferIndex READ selectedBufferIndex WRITE selectedBufferIndexSet NOTIFY selectedBufferChanged)
    Q_PROPERTY(NickListFilter* selectedBufferNicks READ selectedBufferNicks CONSTANT)
    Q_PROPERTY(QAbstractItemModel* logger READ logger CONSTANT)
    Q_PROPERTY(Search* search READ search CONSTANT)

public:
    static Lith *_self;
    static Lith *instance();

    Weechat *weechat();

    QString statusString() const;

    QString errorStringGet();
    void errorStringSet(const QString &o);
    void networkErrorStringSet(const QString &o);

    Settings* settingsGet();
    QAbstractItemModel *logger();
    Search* search();

    void log(Logger::EventType type, QString summary) {
        m_logger->log(Logger::Event{type, summary});
    }
    void log(Logger::EventType type, QString context, QString summary, QString details = QString()) {
        m_logger->log(Logger::Event{type, context, summary, details});
    }

    ProxyBufferList *buffers();
    QmlObjectList *unfilteredBuffers();
    Buffer *selectedBuffer();
    void selectedBufferSet(Buffer *b);
    int selectedBufferIndex();
    void selectedBufferIndexSet(int index);
    NickListFilter *selectedBufferNicks();
    Q_INVOKABLE void switchToBufferNumber(int number);

    // TODO hack, this shouldn't be in this class
    Q_INVOKABLE QString getLinkFileExtension(const QString &url);

public slots:
    void resetData();
    void reconnect();

    void handleBufferInitialization(const Protocol::HData &hda);
    void handleFirstReceivedLine(const Protocol::HData &hda);
    void handleHotlistInitialization(const Protocol::HData &hda);
    void handleNicklistInitialization(const Protocol::HData &hda);

    void handleFetchLines(const Protocol::HData &hda);
    void handleHotlist(const Protocol::HData &hda);

    void _buffer_opened(const Protocol::HData &hda);
    void _buffer_type_changed(const Protocol::HData &hda);
    void _buffer_moved(const Protocol::HData &hda);
    void _buffer_merged(const Protocol::HData &hda);
    void _buffer_unmerged(const Protocol::HData &hda);
    void _buffer_hidden(const Protocol::HData &hda);
    void _buffer_unhidden(const Protocol::HData &hda);
    void _buffer_renamed(const Protocol::HData &hda);
    void _buffer_title_changed(const Protocol::HData &hda);
    void _buffer_localvar_added(const Protocol::HData &hda);
    void _buffer_localvar_changed(const Protocol::HData &hda);
    void _buffer_localvar_removed(const Protocol::HData &hda);
    void _buffer_closing(const Protocol::HData &hda);
    void _buffer_cleared(const Protocol::HData &hda);
    void _buffer_line_added(const Protocol::HData &hda);
    void _nicklist(const Protocol::HData &hda);
    void _nicklist_diff(const Protocol::HData &hda);
    void _pong(const FormattedString &str);

public:
    const Buffer *getBuffer(pointer_t ptr) const;
    const BufferLine *getLine(pointer_t bufPtr, pointer_t linePtr) const;
    const HotListItem *getHotlist(pointer_t ptr) const;

protected:
    void addBuffer(pointer_t ptr, Buffer *b);
    void removeBuffer(pointer_t ptr);
    Buffer *getBuffer(pointer_t ptr);
    void addLine(pointer_t bufPtr, pointer_t linePtr, BufferLine *line);
    BufferLine *getLine(pointer_t bufPtr, pointer_t linePtr);
    void addHotlist(pointer_t ptr, HotListItem *hotlist);
    HotListItem *getHotlist(pointer_t ptr);

signals:
    void selectedBufferChanged();
    void errorStringChanged();

    void pongReceived(qint64 id);

private:
    explicit Lith(QObject *parent = 0);

#ifndef Q_OS_WASM
    QThread *m_weechatThread { nullptr };
#endif
    Weechat *m_weechat { nullptr };
    QmlObjectList *m_buffers { nullptr };
    ProxyBufferList *m_proxyBufferList { nullptr };
    NickListFilter *m_selectedBufferNicks { nullptr };
    MessageFilterList *m_messageBufferList { nullptr };
    Logger *m_logger { nullptr };
    FilteredLogger *m_filteredLogger { nullptr };
    Search *m_search { nullptr };
    int m_selectedBufferIndex { -1 };

    QString m_lastNetworkError {};
    QString m_error {};

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
