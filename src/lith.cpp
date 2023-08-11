// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
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

#include "lith.h"

#include "datamodel.h"
#include "weechat.h"
#include "windowhelper.h"
#include "cmakedefs.h"

#include <iostream>
#include <QThread>
#include <QEventLoop>
#include <QAbstractEventDispatcher>

#include <QSystemTrayIcon>

#include <QUrl>

#if defined(__linux) && !defined(__ANDROID_API__)
#include <QDBusInterface>
#endif // __linux

Lith *Lith::_self = nullptr;
Lith *Lith::instance() {
    if (!_self)
        _self = new Lith();
    return _self;
}

ProxyBufferList *Lith::buffers() {
    return m_proxyBufferList;
}

QmlObjectList *Lith::unfilteredBuffers() {
    return m_buffers;
}

Buffer *Lith::selectedBuffer() {
    if (m_selectedBufferIndex >=0 && m_selectedBufferIndex < m_buffers->count())
        return m_buffers->get<Buffer>(m_selectedBufferIndex);
    return nullptr;
}

void Lith::selectedBufferSet(Buffer *b) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto it = m_buffers->get<Buffer>(i);
        if (it && it == b) {
            selectedBufferIndexSet(i);
            return;
        }
    }
    selectedBufferIndexSet(-1);
}

int Lith::selectedBufferIndex() {
    return m_selectedBufferIndex;
}

void Lith::selectedBufferIndexSet(int index) {
    if (m_selectedBufferIndex != index && index < m_buffers->count()) {
        m_selectedBufferIndex = index;
        emit selectedBufferChanged();
        if (selectedBuffer()) {
            selectedBuffer()->clearHotlist();
        }
        if (index >= 0)
            settingsGet()->lastOpenBufferSet(index);
    }
}

NickListFilter *Lith::selectedBufferNicks() {
    return m_selectedBufferNicks;
}

void Lith::switchToBufferNumber(int number) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto b = m_buffers->get<Buffer>(i);
        if (b && b->numberGet() == number) {
            selectedBufferIndexSet(i);
            break;
        }
    }
}

QString Lith::getLinkFileExtension(const QString &url) {
    QUrl u(url);
    auto extension = u.fileName().split(".").last().toLower();
    return extension;
}

Weechat *Lith::weechat() {
    return m_weechat;
}

QString Lith::statusString() const {
    auto key = QString(QMetaEnum::fromType<Status>().valueToKey(statusGet()));
    key = key.toLower();
    key.front() = key.front().toUpper();
    return key;
}

QString Lith::errorStringGet() {
    return m_error;
}

void Lith::errorStringSet(const QString &o) {
    if (m_error != o) {
        m_error = o;
        emit errorStringChanged();
    }
}

void Lith::networkErrorStringSet(const QString &o) {
    if (m_error != o && m_lastNetworkError != o) {
        m_lastNetworkError = o;
        m_error = o;
        emit errorStringChanged();
    }
}

Settings *Lith::settingsGet() {
    return Settings::instance();
}

QAbstractItemModel *Lith::logger() {
    //return m_logger;
    return m_filteredLogger;
}

Search *Lith::search() {
    return m_search;
}

BaseNetworkProxy *Lith::networkProxy() {
    return m_networkProxy;
}

NotificationHandler *Lith::notificationHandler() {
    return m_notificationHandler;
}

QString Lith::gitVersion() const {
    return GIT_STATE;
}

bool Lith::debugVersion() const {
    return QStringLiteral("Debug") == CMAKE_BUILD_TYPE;
}

Lith::Lith(QObject *parent)
    : QObject(parent)
    , m_windowHelper(new WindowHelper(this))
#ifndef Q_OS_WASM
    , m_weechatThread(new QThread(this))
#endif
    , m_networkProxy(BaseNetworkProxy::create(this))
    , m_weechat(new Weechat(m_networkProxy, this))
    , m_buffers(QmlObjectList::create<Buffer>())
    , m_proxyBufferList(new ProxyBufferList(this, m_buffers))
    , m_selectedBufferNicks(new NickListFilter(this))
    , m_logger(new Logger(this))
    , m_filteredLogger(new FilteredLogger(this))
    , m_search(new Search(this))
    , m_notificationHandler(new NotificationHandler(this))
{
    connect(m_notificationHandler, &NotificationHandler::bufferSelected, this, [this](int bufferNumber) {
        selectBufferNumber(bufferNumber);
    });
    connect(this, &Lith::selectedBufferChanged, [this](){
        if (selectedBuffer())
            m_selectedBufferNicks->setSourceModel(selectedBuffer()->nicks());
        else
            m_selectedBufferNicks->setSourceModel(nullptr);
    });
#ifndef Q_OS_WASM
    m_weechat->moveToThread(m_weechatThread);
    m_weechatThread->start();
#endif
    QTimer::singleShot(1, m_weechat, &Weechat::init);
    QTimer::singleShot(1, this, [this]() {
        m_filteredLogger->setSourceModel(m_logger);
    });
}

void Lith::resetData() {
    selectedBufferIndexSet(-1);

    m_buffers->clear();
    if (!m_logBuffer) {
        m_logBuffer = QSharedPointer<Buffer>::create(this, 0);
        m_logBuffer->nameSet("Lith Log");
        auto eventToLine = [](Buffer *parent, const QDateTime& dateTime, const Logger::Event& event) -> BufferLine* {
            auto logLine = new BufferLine(parent);
            logLine->dateSet(dateTime);
            QMetaEnum me = QMetaEnum::fromType<Logger::EventType>();
            auto type = QString(me.valueToKey(event.type)).toUpper();
            logLine->prefixSet(type);
            FormattedString msg;
            if (!event.context.isEmpty()) {
                msg.addPart(FormattedString::Part(event.context));
                msg.lastPart().bold = true;
                msg.addPart(FormattedString::Part(" " + event.summary));
            }
            else {
                msg.addPart(FormattedString::Part(event.summary));
            }
            if (!event.details.isEmpty()) {
                msg.lastPart().text.append("\n");
                msg.addPart(FormattedString::Part(event.details));
                msg.lastPart().foreground.index = 15;
            }
            logLine->messageSet(msg);
            return logLine;
        };
        if (settingsGet()->enableLoggingGet()) {
            for (const auto& eventPair : m_logger->events()) {
                m_logBuffer->prependLine(eventToLine(m_logBuffer.data(), eventPair.first, eventPair.second));
            }
        }
        connect(m_logger, &Logger::eventAdded, this, [this, eventToLine](const QDateTime& dateTime, const Logger::Event& event) {
            if (settingsGet()->enableLoggingGet()) {
                m_logBuffer->prependLine(eventToLine(m_logBuffer.data(), dateTime, event));
            }
        });
        connect (settingsGet(), &Settings::enableLoggingChanged, this, [this]() {
            if (settingsGet()->enableLoggingGet()) {
                auto previousSelectedBuffer = selectedBuffer();
                m_buffers->prepend(m_logBuffer);
                selectedBufferSet(previousSelectedBuffer);
            }
            else {
                if (selectedBuffer() == m_logBuffer.data())
                    selectedBufferIndexSet(-1);
                m_buffers->removeItem(m_logBuffer.data());
            }
        });
    }
    if (settingsGet()->enableLoggingGet())
        m_buffers->prepend(m_logBuffer);
    m_bufferMap.clear();
    m_lineMap.clear();
    m_hotList.clear();
}

void Lith::reconnect() {
    // Network code lives in a different thread
    // TODO use signals/slots to make it cleaner
    QMetaObject::invokeMethod(m_weechat, "restart", Qt::QueuedConnection);
}

void Lith::selectBufferNumber(int bufferNumber) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto buffer = m_buffers->get<Buffer>(i);
        if (buffer && buffer->numberGet() == bufferNumber) {
            selectedBufferSet(buffer);
            return;
        }
    }
}

void Lith::handleBufferInitialization(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer
        auto ptr = i.pointers.first();
        auto b = new Buffer(this, ptr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            b->setProperty(qPrintable(key), value);
        }
        addBuffer(ptr, b);
    }
}

void Lith::handleFirstReceivedLine(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer - lines - line - line_data
        auto bufPtr = i.pointers.first();
        auto linePtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto line = getLine(bufPtr, linePtr);
        if (line)
            continue;
        line = new BufferLine(buffer);
        line->ptrSet(linePtr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key != "buffer")
                line->setProperty(qPrintable(key), value);
        }
        buffer->appendLine(line);
        addLine(bufPtr, linePtr, line);
    }
}

void Lith::handleHotlistInitialization(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // hotlist
        auto ptr = i.pointers.first();
        auto bufPtr = i.objects["buffer"].value<pointer_t>();
        auto item = new HotListItem(this);
        auto buffer = getBuffer(bufPtr);
        if (buffer) {
            item->bufferSet(buffer);
        }
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key == "buffer")
                continue;
            item->setProperty(qPrintable(key), value);
        }
        addHotlist(ptr, item);
    }
}

void Lith::handleNicklistInitialization(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Nick missing a parent:";
            continue;
        }
        auto nick = new Nick(buffer);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            nick->setProperty(qPrintable(key), value);
        }
        buffer->addNick(nickPtr, nick);
    }
}

void Lith::handleFetchLines(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer - lines - line - line_data
        auto bufPtr = i.pointers.first();
        auto linePtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto line = getLine(bufPtr, linePtr);
        if (line)
            continue;
        line = new BufferLine(buffer);
        line->ptrSet(linePtr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key != "buffer")
                line->setProperty(qPrintable(key), value);
        }
        buffer->appendLine(line);
        addLine(bufPtr, linePtr, line);
    }
}

void Lith::handleHotlist(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // hotlist
        auto hlPtr = i.pointers.first();
        auto bufPtr = qvariant_cast<pointer_t>(i.objects["buffer"]);
        auto hl = getHotlist(hlPtr);
        auto buf = getBuffer(bufPtr);
        if (!buf) {
            qWarning() << "Got a hotlist item" << QString("%1").arg(hlPtr, 16, 16, QChar('0')) <<  "for nonexistent buffer" << QString("%1").arg(bufPtr, 16, 16, QChar('0'));
            continue;
        }
        if (!hl) {
            hl = new HotListItem(this);
            hl->bufferSet(buf);
        }
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key == "buffer")
                continue;
            hl->setProperty(qPrintable(key), value);
        }
    }
}

void Lith::_buffer_opened(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buffer = getBuffer(bufPtr);
        if (buffer)
            continue;
        buffer = new Buffer(this, bufPtr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            buffer->setProperty(qPrintable(key), value);
        }
        addBuffer(bufPtr, buffer);
    }
}

void Lith::_buffer_type_changed(const WeeChatProtocol::HData &hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_moved(const WeeChatProtocol::HData &hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_merged(const WeeChatProtocol::HData &hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_unmerged(const WeeChatProtocol::HData &hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_hidden(const WeeChatProtocol::HData &hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_unhidden(const WeeChatProtocol::HData &hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_renamed(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buf = getBuffer(bufPtr);
        if (!buf)
            continue;
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key.endsWith("name")) {
                buf->setProperty(qPrintable(key), value);
            }
        }
    }
}

void Lith::_buffer_title_changed(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buf = getBuffer(bufPtr);
        if (!buf)
            continue;
        buf->titleSet(qvariant_cast<FormattedString>(i.objects["title"]));
    }
}

void Lith::_buffer_localvar_added(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buf = getBuffer(bufPtr);
        if (!buf)
            continue;
        auto strm = i.objects["local_variables"].value<StringMap>();
        buf->local_variablesSet(strm);
    }
}

void Lith::_buffer_localvar_changed(const WeeChatProtocol::HData &hda) {
    // These three seem to be the same
    _buffer_localvar_added(hda);
}

void Lith::_buffer_localvar_removed(const WeeChatProtocol::HData &hda) {
    // These three seem to be the same
    _buffer_localvar_added(hda);
}

void Lith::_buffer_closing(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto buffer = getBuffer(bufPtr);
        if (!buffer)
            continue;

        buffer->deleteLater();
        removeBuffer(bufPtr);
    }
}

void Lith::_buffer_cleared(const WeeChatProtocol::HData &hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    std::cerr << hda.toString().toStdString() << std::endl;
}

void Lith::_buffer_line_added(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // line_data
        auto linePtr = i.pointers.last();
        // path doesn't contain the buffer, we need to retrieve it like this
        auto bufPtr = qvariant_cast<pointer_t>(i.objects["buffer"]);
        auto buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto oldLine = getLine(bufPtr, linePtr);
        if (oldLine) {
            oldLine->ptrSet(0);
        }
        auto line = new BufferLine(buffer);
        line->ptrSet(linePtr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key == "buffer")
                continue;
            line->setProperty(qPrintable(key), value);
        }
        m_logger->log(Logger::Event{Logger::LineAdded, buffer->nameGet(), "Received a line", QString("Nick: %1, Line: %2").arg(line->nickGet()).arg(line->messageGet())});
        buffer->prependLine(line);
        // Overwrite the duplicate map here in case it already exists, we've hit the limit of WeeChat can return (usually 4096 lines per buffer)
        addLine(bufPtr, linePtr, line, true);
        if (settingsGet()->enableNotificationsGet() && (line->highlightGet() || (buffer->isPrivateGet() && line->isPrivMsgGet() && !line->isSelfMsgGet()))) {
            QString title;
            if (buffer->isChannelGet() || buffer->isServerGet()) {
                title = tr("New highlight in %1 from %2").arg(buffer->short_nameGet()).arg(line->colorlessNicknameGet());
            }
            else {
                title = tr("New message from %1").arg(buffer->short_nameGet());
            }

#if defined(__linux) && !defined(__ANDROID_API__)
            QDBusInterface notifications("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications", QDBusConnection::sessionBus());
            auto reply = notifications.call("Notify", "Lith", 0U, "org.LithApp.Lith", title, line->colorlessTextGet(), QStringList{}, QVariantMap{}, -1);
#else
            static QIcon appIcon(":/icon.png");
            static QSystemTrayIcon *icon = new QSystemTrayIcon(appIcon);
            icon->show();

            icon->showMessage(title, line->colorlessTextGet(), appIcon);
#endif // __linux
        }
    }
}

void Lith::_nicklist(const WeeChatProtocol::HData &hda) {
    Buffer *previousBuffer = nullptr;
    for (auto &i : hda.data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer)
            continue;
        if (buffer != previousBuffer)
            buffer->clearNicks();
        previousBuffer = buffer;
        auto nick = new Nick(buffer);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            nick->setProperty(qPrintable(key), value);
        }
        buffer->addNick(nickPtr, nick);
    }
}

void Lith::_nicklist_diff(const WeeChatProtocol::HData &hda) {
    for (auto &i : hda.data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto buffer = getBuffer(bufPtr);
        if (!buffer)
            continue;
        auto op = qvariant_cast<char>(i.objects["_diff"]);
        switch (op) {
        case '+': {
            auto nick = new Nick(buffer);
            for (auto [key, value] : i.objects.asKeyValueRange()) {
                if (key == "_diff")
                    continue;
                nick->setProperty(qPrintable(key), value);
            }
            buffer->addNick(nickPtr, nick);
            break;
        }
        case '-': {
            buffer->removeNick(nickPtr);
            break;
        }
        case '^':
        case '*': {
            auto nick = buffer->getNick(nickPtr);
            if (!nick)
                break;
            for (auto [key, value] : i.objects.asKeyValueRange()) {
                if (key == "_diff")
                    continue;
                nick->setProperty(qPrintable(key), value);
            }
            break;
        }
        default:
            break;
        }

    }
}

void Lith::_pong(const FormattedString &str) {
    emit pongReceived(str.toLongLong());
}

void Lith::addBuffer(pointer_t ptr, Buffer *b) {
    m_bufferMap[ptr] = b;
    m_buffers->append(b);
    auto lastOpenBuffer = settingsGet()->lastOpenBufferGet();
    if (m_buffers->count() == 1 && lastOpenBuffer < 0)
        emit selectedBufferChanged();
    else if (lastOpenBuffer == m_buffers->count() - 1) {
        selectedBufferIndexSet(lastOpenBuffer);
    }
}

void Lith::removeBuffer(pointer_t ptr) {
    if (m_bufferMap.contains(ptr)) {
        auto buf = m_bufferMap[ptr];
        if (selectedBuffer() == buf)
            selectedBufferIndexSet(selectedBufferIndex() - 1);
        m_bufferMap.remove(ptr);
        m_buffers->removeItem(buf);
    }
}

Buffer *Lith::getBuffer(pointer_t ptr) {
    if (m_bufferMap.contains(ptr))
        return m_bufferMap[ptr];
    return nullptr;
}

void Lith::addLine(pointer_t bufPtr, pointer_t linePtr, BufferLine *line, bool overwrite) {
    if (m_lineMap.contains(bufPtr) && m_lineMap[bufPtr].contains(linePtr) && !overwrite) {
        qCritical() << "Line with bufPtr" << QString("%1").arg(bufPtr, 16, 16, QChar('0')) << "and linePtr" << QString("%1").arg(linePtr, 16, 16, QChar('0')) << "already exists";
        qCritical() << "Original: " << m_lineMap[bufPtr][linePtr]->messageGet();
        qCritical() << "New:" << line->messageGet();
        return;
    }
    m_lineMap[bufPtr][linePtr] = line;
}

BufferLine *Lith::getLine(pointer_t bufPtr, pointer_t linePtr) {
    if (m_lineMap.contains(bufPtr) && m_lineMap[bufPtr].contains(linePtr)) {
        return m_lineMap[bufPtr][linePtr];
    }
    return nullptr;
}

const BufferLine *Lith::getLine(pointer_t bufPtr, pointer_t linePtr) const {
    if (m_lineMap.contains(bufPtr) && m_lineMap.value(bufPtr).contains(linePtr)) {
        return m_lineMap.value(bufPtr).value(linePtr);
    }
    return nullptr;
}

void Lith::addHotlist(pointer_t ptr, HotListItem *hotlist) {
    if (m_hotList.contains(ptr)) {
        // TODO
        qCritical() << "Hotlist with ptr" << QString("%1").arg(ptr, 8, 16, QChar('0')) << "already exists";
    }
    m_hotList[ptr] = hotlist;
}

HotListItem *Lith::getHotlist(pointer_t ptr) {
    if (m_hotList.contains(ptr))
        return m_hotList[ptr];
    return nullptr;
}

const Buffer *Lith::getBuffer(pointer_t ptr) const {
    if (m_bufferMap.contains(ptr))
        return m_bufferMap[ptr];
    return nullptr;
}

const HotListItem *Lith::getHotlist(pointer_t ptr) const {
    if (m_hotList.contains(ptr))
        return m_hotList[ptr];
    return nullptr;
}


ProxyBufferList::ProxyBufferList(QObject *parent, QAbstractListModel *parentModel)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(parentModel);
    setFilterRole(Qt::UserRole);
    connect(this, &ProxyBufferList::filterWordChanged, [this] {
        setFilterFixedString(filterWordGet());
    });
}
bool ProxyBufferList::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    auto index = sourceModel()->index(source_row, 0, source_parent);
    auto v = sourceModel()->data(index);
    auto b = qvariant_cast<Buffer*>(v);
    if (b) {
        return b->nameGet().toLower().contains(filterWordGet().toLower());
    }
    return false;
}

