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
#include "replayproxy.h"
#include "nicklistfilter.h"
#include "messagelistfilter.h"
#include "lithcore_export.h"

#include "notificationhandler.h"

#include <QSortFilterProxyModel>
#include <QPointer>

class Weechat;
class ProxyBufferList;

class Buffer;
class BufferLine;
class HotListItem;
class BaseNetworkProxy;
class NotificationHandler;

class LITHCORE_EXPORT Lith : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
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

    Q_PROPERTY(ProxyBufferList* buffers READ buffers CONSTANT)
    Q_PROPERTY(QmlObjectList* unfilteredBuffers READ unfilteredBuffers CONSTANT)
    Q_PROPERTY(Buffer* selectedBuffer READ selectedBuffer WRITE selectedBufferSet NOTIFY selectedBufferChanged)
    Q_PROPERTY(int selectedBufferIndex READ selectedBufferIndex WRITE selectedBufferIndexSet NOTIFY selectedBufferChanged)
    Q_PROPERTY(int mappedSelectedBufferIndex READ mappedSelectedBufferIndex NOTIFY selectedBufferChanged)
    Q_PROPERTY(NickListFilter* selectedBufferNicks READ selectedBufferNicks CONSTANT)
    Q_PROPERTY(QAbstractItemModel* logger READ logger CONSTANT)
    Q_PROPERTY(Search* search READ search CONSTANT)
    Q_PROPERTY(BaseNetworkProxy* networkProxy READ networkProxy CONSTANT)
    Q_PROPERTY(NotificationHandler* notificationHandler READ notificationHandler CONSTANT)

    Q_PROPERTY(QString gitVersion READ gitVersion CONSTANT)
    Q_PROPERTY(bool debugVersion READ debugVersion CONSTANT)
    PROPERTY(bool, showDebugWindow, false)

public:
    ~Lith() override;
    static Lith* instance();
    static Lith* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        Q_UNUSED(qmlEngine)
        Q_UNUSED(jsEngine)
        return instance();
    }

    Weechat* weechat();

    QString statusString() const;

    QString errorStringGet();
    void errorStringSet(const QString& o);
    void networkErrorStringSet(const QString& o);

    static Settings* settingsGet();
    QAbstractItemModel* logger();
    Search* search();
    BaseNetworkProxy* networkProxy();
    NotificationHandler* notificationHandler();

    static QString gitVersion();
    static bool debugVersion();

    void log(Logger::EventType type, QString summary) {
        m_logger->log(Logger::Event {type, std::move(summary)});
    }
    void log(Logger::EventType type, QString context, QString summary, QString details = QString()) {
        m_logger->log(Logger::Event {type, std::move(context), std::move(summary), std::move(details)});
    }

    ProxyBufferList* buffers();
    QmlObjectList* unfilteredBuffers();
    Buffer* selectedBuffer();
    void selectedBufferSet(Buffer* b);
    int selectedBufferIndex() const;
    void selectedBufferIndexSet(int index);
    // Maps index between the source model and the reordered one when selecting and highlighting
    int mappedSelectedBufferIndex() const;
    NickListFilter* selectedBufferNicks();
    Q_INVOKABLE void switchToBufferNumber(int number);

    // TODO hack, this shouldn't be in this class
    Q_INVOKABLE static QString getLinkFileExtension(const QString& url);

public slots:
    void resetData();
    void reconnect();

    void selectBufferNumber(int bufferNumber);

    void handleBufferInitialization(const WeeChatProtocol::HData& hda);
    void handleFirstReceivedLine(const WeeChatProtocol::HData& hda);
    void handleHotlistInitialization(const WeeChatProtocol::HData& hda);
    void handleNicklistInitialization(const WeeChatProtocol::HData& hda);

    void handleFetchLines(const WeeChatProtocol::HData& hda);
    void handleHotlist(const WeeChatProtocol::HData& hda);

    void _buffer_opened(const WeeChatProtocol::HData& hda);
    void _buffer_type_changed(const WeeChatProtocol::HData& hda);
    void _buffer_moved(const WeeChatProtocol::HData& hda);
    void _buffer_merged(const WeeChatProtocol::HData& hda);
    void _buffer_unmerged(const WeeChatProtocol::HData& hda);
    void _buffer_hidden(const WeeChatProtocol::HData& hda);
    void _buffer_unhidden(const WeeChatProtocol::HData& hda);
    void _buffer_renamed(const WeeChatProtocol::HData& hda);
    void _buffer_title_changed(const WeeChatProtocol::HData& hda);
    void _buffer_localvar_added(const WeeChatProtocol::HData& hda);
    void _buffer_localvar_changed(const WeeChatProtocol::HData& hda);
    void _buffer_localvar_removed(const WeeChatProtocol::HData& hda);
    void _buffer_closing(const WeeChatProtocol::HData& hda);
    void _buffer_cleared(const WeeChatProtocol::HData& hda);
    void _buffer_line_added(const WeeChatProtocol::HData& hda);
    void _nicklist(const WeeChatProtocol::HData& hda);
    void _nicklist_diff(const WeeChatProtocol::HData& hda);
    void _pong(const FormattedString& str);

public:
    const Buffer* getBuffer(weechat_pointer_t ptr) const;
    const BufferLine* getLine(weechat_pointer_t bufPtr, weechat_pointer_t linePtr) const;
    const HotListItem* getHotlist(weechat_pointer_t ptr) const;

protected:
    void addBuffer(weechat_pointer_t ptr, Buffer* b);
    void removeBuffer(weechat_pointer_t ptr);
    Buffer* getBuffer(weechat_pointer_t ptr);
    void addLine(weechat_pointer_t bufPtr, weechat_pointer_t linePtr, BufferLine* line);
    BufferLine* getLine(weechat_pointer_t bufPtr, weechat_pointer_t linePtr);
    void addHotlist(weechat_pointer_t ptr, HotListItem* hotlist);
    HotListItem* getHotlist(weechat_pointer_t ptr);

signals:
    void selectedBufferChanged();
    void errorStringChanged();
    void hotlistUpdated();

    void pongReceived(qint64 id);

private:
    explicit Lith(QObject* parent = nullptr);

#ifndef Q_OS_WASM
    QThread* m_weechatThread {nullptr};
#endif
    BaseNetworkProxy* m_networkProxy {nullptr};
    Weechat* m_weechat {nullptr};
    QmlObjectList* m_buffers {nullptr};
    ProxyBufferList* m_proxyBufferList {nullptr};
    NickListFilter* m_selectedBufferNicks {nullptr};
    MessageFilterList* m_messageBufferList {nullptr};
    Logger* m_logger {nullptr};
    FilteredLogger* m_filteredLogger {nullptr};
    Search* m_search {nullptr};
    NotificationHandler* m_notificationHandler {nullptr};
    int m_selectedBufferIndex {-1};

    QString m_lastNetworkError {};
    QString m_error {};

    QMap<weechat_pointer_t, QPointer<Buffer>> m_bufferMap {};
    QMap<weechat_pointer_t, QMap<weechat_pointer_t, QPointer<BufferLine>>> m_lineMap;
    QMap<weechat_pointer_t, QPointer<HotListItem>> m_hotList;

    Buffer* m_logBuffer = nullptr;
};

class LITHCORE_EXPORT ProxyBufferList : public QSortFilterProxyModel {
    Q_OBJECT
    PROPERTY(QString, filterWord)
    PROPERTY(bool, showOnlyBuffersWithNewMessages, false)
public:
    explicit ProxyBufferList(Lith* parent = nullptr, QAbstractListModel* parentModel = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;
};

#endif  // LITH_H
