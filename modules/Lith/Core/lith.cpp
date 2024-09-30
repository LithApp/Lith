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

#include "defs/cmakedefs.h"
#include "datamodel.h"
#include "weechat.h"
#include "windowhelper.h"

#include <QAbstractEventDispatcher>
#include <QEventLoop>
#include <QThread>
#include <iostream>

#include <QSystemTrayIcon>

#include <QUrl>

#if defined(__linux) && !defined(__ANDROID_API__)
  #include <QDBusInterface>
  #include <QtDBus/QDBusConnectionInterface>
#endif  // __linux

Lith::~Lith() {
#ifndef __EMSCRIPTEN__
    m_weechatThread->quit();
    m_weechatThread->wait();
#endif  // __EMSCRIPTEN__
}

Lith* Lith::instance() {
    static Lith* _self = nullptr;
    if (!_self) {
        Q_INIT_RESOURCE(assets);

        // Register types
        QMetaType::registerConverter<FormattedString, QString>([](const FormattedString& s) {
            if (s.containsHtml()) {
                if (WindowHelper::instance()->lightThemeGet()) {
                    return s.toHtml(*lightTheme);
                } else {
                    return s.toHtml(*darkTheme);
                }
            }
            return s.toPlain();
        });
        _self = new Lith();
    }
    return _self;
}

ProxyBufferList* Lith::buffers() {
    return m_proxyBufferList;
}

QmlObjectList* Lith::unfilteredBuffers() {
    return m_buffers;
}

Buffer* Lith::selectedBuffer() {
    if (m_selectedBufferIndex >= 0 && m_selectedBufferIndex < m_buffers->count()) {
        return m_buffers->get<Buffer>(m_selectedBufferIndex);
    }
    return nullptr;
}

void Lith::selectedBufferSet(Buffer* b) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto* it = m_buffers->get<Buffer>(i);
        if (it && it == b) {
            selectedBufferIndexSet(i);
            return;
        }
    }
    selectedBufferIndexSet(-1);
}

int Lith::selectedBufferIndex() const {
    return m_selectedBufferIndex;
}

void Lith::selectedBufferIndexSet(int index) {
    if (m_selectedBufferIndex != index && index < m_buffers->count()) {
        m_selectedBufferIndex = index;
        emit selectedBufferChanged();
        if (selectedBuffer()) {
            selectedBuffer()->clearHotlist();
        }
        if (index >= 0) {
            settingsGet()->lastOpenBufferSet(index);
        }
    }
}

int Lith::mappedSelectedBufferIndex() const {
    if (m_selectedBufferIndex < 0) {
        return -1;
    }
    auto mapped = m_proxyBufferList->mapFromSource(m_buffers->index(m_selectedBufferIndex, 0));
    if (mapped.isValid()) {
        return mapped.row();
    }
    return -1;
}

NickListFilter* Lith::selectedBufferNicks() {
    return m_selectedBufferNicks;
}

void Lith::switchToBufferNumber(int number) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto* b = m_buffers->get<Buffer>(i);
        if (b && b->numberGet() == number) {
            selectedBufferIndexSet(i);
            break;
        }
    }
}

QString Lith::getLinkFileExtension(const QString& url) {
    QUrl u(url);
    auto extension = u.fileName().split(QLatin1Char('.')).last().toLower();
    return extension;
}

Weechat* Lith::weechat() {
    return m_weechat;
}

QString Lith::statusString() const {
    auto key = QString::fromLatin1(QMetaEnum::fromType<Status>().valueToKey(statusGet()));
    key = key.toLower();
    key.front() = key.front().toUpper();
    return key;
}

QString Lith::errorStringGet() {
    return m_error;
}

void Lith::errorStringSet(const QString& o) {
    if (m_error != o) {
        m_error = o;
        emit errorStringChanged();
        if (m_error.isEmpty()) {
            statusSet(DISCONNECTED);
        }
    }
}

void Lith::networkErrorStringSet(const QString& o) {
    if (m_error != o && m_lastNetworkError != o) {
        m_lastNetworkError = o;
        m_error = o;
        emit errorStringChanged();
    }
}

Settings* Lith::settingsGet() {
    return Settings::instance();
}

QAbstractItemModel* Lith::logger() {
    // return m_logger;
    return m_filteredLogger;
}

Search* Lith::search() {
    return m_search;
}

BaseNetworkProxy* Lith::networkProxy() {
    return m_networkProxy;
}

NotificationHandler* Lith::notificationHandler() {
    return m_notificationHandler;
}

QString Lith::gitVersion() {
    return QStringLiteral(GIT_STATE);
}

bool Lith::debugVersion() {
    return QStringLiteral("Debug") == QStringLiteral(CMAKE_BUILD_TYPE);
}

Lith::Lith(QObject* parent)
    : QObject(parent)
#ifndef Q_OS_WASM
    , m_weechatThread(new QThread(this))
#endif
    , m_networkProxy(BaseNetworkProxy::create(this))
    , m_weechat(new Weechat(m_networkProxy, this))
    , m_buffers(QmlObjectList::create<Buffer>(
          this, static_cast<QmlObjectList::RoleMode>(QmlObjectList::ModelData | QmlObjectList::Identity), false
      ))
    , m_proxyBufferList(new ProxyBufferList(this, m_buffers))
    , m_selectedBufferNicks(new NickListFilter(this))
    , m_logger(new Logger(this))
    , m_filteredLogger(new FilteredLogger(this))
    , m_search(new Search(this))
    , m_notificationHandler(new NotificationHandler(this)) {
    auto showDebugWindowEnv = qgetenv("LITH_SHOW_DEBUG_WINDOW");
    if (!showDebugWindowEnv.isEmpty() && showDebugWindowEnv != "0") {
        showDebugWindowSet(true);
    }
    connect(m_notificationHandler, &NotificationHandler::bufferSelected, this, [this](int bufferNumber) {
        selectBufferNumber(bufferNumber);
    });
    connect(this, &Lith::selectedBufferChanged, [this]() {
        if (selectedBuffer()) {
            m_selectedBufferNicks->setSourceModel(selectedBuffer()->nicks());
        } else {
            m_selectedBufferNicks->setSourceModel(nullptr);
        }
    });
#ifndef Q_OS_WASM
    m_weechat->moveToThread(m_weechatThread);
    m_weechatThread->start();
#endif
    QTimer::singleShot(1, m_weechat, &Weechat::init);
    QTimer::singleShot(1, this, [this]() { m_filteredLogger->setSourceModel(m_logger); });
}

void Lith::resetData() {
    selectedBufferIndexSet(-1);

    m_buffers->clear();
    if (!m_logBuffer) {
        m_logBuffer = new Buffer(this, 0);
        m_logBuffer->nameSet(FormattedString(QStringLiteral("Lith Log")));
        auto eventToLine = [](Buffer* parent, const QDateTime& dateTime, const Logger::Event& event) -> BufferLine* {
            auto* logLine = new BufferLine(parent);
            logLine->dateSet(dateTime);
            QMetaEnum me = QMetaEnum::fromType<Logger::EventType>();
            auto type = QString::fromLatin1(me.valueToKey(event.type)).toUpper();
            logLine->prefixSet(FormattedString(std::move(type)));
            FormattedString msg;
            if (!event.context.isEmpty()) {
                msg.addPart(event.context);
                msg.lastPart().bold = true;
                msg.addPart(QStringLiteral(" ") + event.summary);
            } else {
                msg.addPart(event.summary);
            }
            if (!event.details.isEmpty()) {
                msg.addChar(QByteArrayLiteral("\n"));
                msg.addPart(event.details);
                msg.lastPart().foreground.index = 15;
            }
            logLine->messageSet(msg);
            return logLine;
        };
        if (settingsGet()->enableLoggingGet()) {
            for (const auto& eventPair : m_logger->events()) {
                m_logBuffer->prependLine(eventToLine(m_logBuffer, eventPair.first, eventPair.second));
            }
        }
        connect(m_logger, &Logger::eventAdded, this, [this, eventToLine](const QDateTime& dateTime, const Logger::Event& event) {
            if (settingsGet()->enableLoggingGet()) {
                m_logBuffer->prependLine(eventToLine(m_logBuffer, dateTime, event));
            }
        });
        connect(settingsGet(), &Settings::enableLoggingChanged, this, [this]() {
            if (settingsGet()->enableLoggingGet()) {
                auto* previousSelectedBuffer = selectedBuffer();
                m_buffers->prepend(m_logBuffer);
                selectedBufferSet(previousSelectedBuffer);
            } else {
                if (selectedBuffer() == m_logBuffer) {
                    selectedBufferIndexSet(-1);
                }
                m_buffers->removeItem(m_logBuffer);
            }
        });
    }
    if (settingsGet()->enableLoggingGet()) {
        m_buffers->prepend(m_logBuffer);
    }
    m_bufferMap.clear();
    m_lineMap.clear();
    m_hotList.clear();

    connect(Lith::settingsGet(), &Settings::bufferListGroupingByServerChanged, this, [this]() { emit selectedBufferChanged(); });
    connect(Lith::settingsGet(), &Settings::bufferListAlphabeticalSortingChanged, this, [this]() { emit selectedBufferChanged(); });
}

void Lith::reconnect() {
    // Network code lives in a different thread
    // TODO use signals/slots to make it cleaner
    QMetaObject::invokeMethod(m_weechat, "userRequestedRestart", Qt::QueuedConnection);
}

void Lith::selectBufferNumber(int bufferNumber) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto* buffer = m_buffers->get<Buffer>(i);
        if (buffer && buffer->numberGet() == bufferNumber) {
            selectedBufferSet(buffer);
            return;
        }
    }
}

void Lith::handleBufferInitialization(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer
        auto ptr = i.pointers.first();
        auto* b = new Buffer(this, ptr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            b->setProperty(qPrintable(key), value);
        }
        addBuffer(ptr, b);
    }
}

void Lith::handleFirstReceivedLine(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer - lines - line - line_data
        auto bufPtr = i.pointers.first();
        auto linePtr = i.pointers.last();
        auto* buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto* line = getLine(bufPtr, linePtr);
        if (line) {
            continue;
        }
        line = new BufferLine(buffer);
        line->ptrSet(linePtr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key != QStringLiteral("buffer")) {
                line->setProperty(qPrintable(key), value);
            }
        }
        buffer->appendLine(line);
        addLine(bufPtr, linePtr, line);
    }
}

void Lith::handleHotlistInitialization(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // hotlist
        auto ptr = i.pointers.first();
        auto bufPtr = i.objects[QStringLiteral("buffer")].value<weechat_pointer_t>();
        auto item = new HotListItem(this);
        auto* buffer = getBuffer(bufPtr);
        if (buffer) {
            item->bufferSet(buffer);
        }
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key == QStringLiteral("buffer")) {
                continue;
            }
            item->setProperty(qPrintable(key), value);
        }
        addHotlist(ptr, item);
    }
    emit hotlistUpdated();
}

void Lith::handleNicklistInitialization(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto* buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Nick missing a parent:";
            continue;
        }
        auto* nick = new Nick(buffer);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            nick->setProperty(qPrintable(key), value);
        }
        buffer->addNick(nickPtr, nick);
    }
}

void Lith::handleFetchLines(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer - lines - line - line_data
        auto bufPtr = i.pointers.first();
        auto linePtr = i.pointers.last();
        auto* buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto* line = getLine(bufPtr, linePtr);
        if (line) {
            continue;
        }
        line = new BufferLine(buffer);
        line->ptrSet(linePtr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key != QStringLiteral("buffer")) {
                line->setProperty(qPrintable(key), value);
            }
        }
        buffer->appendLine(line);
        addLine(bufPtr, linePtr, line);
    }
}

void Lith::handleHotlist(const WeeChatProtocol::HData& hda) {
    // Server only sends entries for buffers that have a hotlist, others need to be cleared
    QSet<HotListItem*> done;
    for (const auto& i : hda.data) {
        // hotlist
        auto hlPtr = i.pointers.first();
        auto bufPtr = qvariant_cast<weechat_pointer_t>(i.objects.value(QStringLiteral("buffer")));
        auto* hl = getHotlist(hlPtr);
        auto* buf = getBuffer(bufPtr);
        if (!buf) {
            qWarning() << QStringLiteral("Got a hotlist item") << QStringLiteral("%1").arg(hlPtr, 16, 16, QLatin1Char('0'))
                       << QStringLiteral("for nonexistent buffer") << QStringLiteral("%1").arg(bufPtr, 16, 16, QLatin1Char('0'));
            continue;
        }
        if (!hl) {
            hl = new HotListItem(this);
            hl->bufferSet(buf);
            addHotlist(hlPtr, hl);
        }
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key == QStringLiteral("buffer")) {
                continue;
            }
            hl->setProperty(qPrintable(key), value);
        }
        done.insert(hl);
    }
    for (auto& i : m_hotList) {
        if (!done.contains(i)) {
            i->countSet({});
        }
    }
    emit hotlistUpdated();
}

void Lith::_buffer_opened(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto* buffer = getBuffer(bufPtr);
        if (buffer) {
            continue;
        }
        buffer = new Buffer(this, bufPtr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            buffer->setProperty(qPrintable(key), value);
        }
        addBuffer(bufPtr, buffer);
    }
}

// NOLINTBEGIN
void Lith::_buffer_type_changed(const WeeChatProtocol::HData& hda) {
    Q_UNUSED(hda)
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_moved(const WeeChatProtocol::HData& hda) {
    Q_UNUSED(hda)
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_merged(const WeeChatProtocol::HData& hda) {
    Q_UNUSED(hda)
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_unmerged(const WeeChatProtocol::HData& hda) {
    Q_UNUSED(hda)
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_hidden(const WeeChatProtocol::HData& hda) {
    Q_UNUSED(hda)
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_unhidden(const WeeChatProtocol::HData& hda) {
    Q_UNUSED(hda)
    qCritical() << __FUNCTION__ << "is not implemented yet";
}

void Lith::_buffer_cleared(const WeeChatProtocol::HData& hda) {
    qCritical() << __FUNCTION__ << "is not implemented yet";
    std::cerr << hda.toString().toStdString() << std::endl;
}
// NOLINTEND

void Lith::_buffer_renamed(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto* buf = getBuffer(bufPtr);
        if (!buf) {
            continue;
        }
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key.endsWith(QStringLiteral("name"))) {
                buf->setProperty(qPrintable(key), value);
            }
        }
    }
}

void Lith::_buffer_title_changed(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto* buf = getBuffer(bufPtr);
        if (!buf) {
            continue;
        }
        buf->titleSet(qvariant_cast<FormattedString>(i.objects[QStringLiteral("title")]));
    }
}

void Lith::_buffer_localvar_added(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto* buf = getBuffer(bufPtr);
        if (!buf) {
            continue;
        }
        auto strm = i.objects[QStringLiteral("local_variables")].value<StringMap>();
        buf->local_variablesSet(strm);
    }
}

void Lith::_buffer_localvar_changed(const WeeChatProtocol::HData& hda) {
    // These three seem to be the same
    _buffer_localvar_added(hda);
}

void Lith::_buffer_localvar_removed(const WeeChatProtocol::HData& hda) {
    // These three seem to be the same
    _buffer_localvar_added(hda);
}

void Lith::_buffer_closing(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer
        auto bufPtr = i.pointers.first();
        auto* buffer = getBuffer(bufPtr);
        if (!buffer) {
            continue;
        }

        buffer->deleteLater();
        removeBuffer(bufPtr);
    }
}

void Lith::_buffer_line_added(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // line_data
        auto linePtr = i.pointers.last();
        // path doesn't contain the buffer, we need to retrieve it like this
        auto bufPtr = qvariant_cast<weechat_pointer_t>(i.objects[QStringLiteral("buffer")]);
        auto* buffer = getBuffer(bufPtr);
        if (!buffer) {
            qWarning() << "Line missing a parent:";
            continue;
        }
        auto* oldLine = getLine(bufPtr, linePtr);
        if (oldLine) {
            oldLine->ptrSet(0);
        }
        auto* line = new BufferLine(buffer);
        line->ptrSet(linePtr);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            if (key == QStringLiteral("buffer")) {
                continue;
            }
            line->setProperty(qPrintable(key), value);
        }
        m_logger->log(Logger::Event {
            Logger::LineAdded, buffer->nameGet().toPlain(), QStringLiteral("Received a line"),
            QStringLiteral("Nick: %1, Line: %2").arg(line->nickGet()).arg(line->messageGet().toPlain())
        });
        buffer->prependLine(line);
        // Overwrite the duplicate map here in case it already exists, we've hit the
        // limit of WeeChat can return (usually 4096 lines per buffer)
        addLine(bufPtr, linePtr, line);
        if (settingsGet()->enableNotificationsGet() &&
            (line->highlightGet() || (buffer->isPrivateGet() && line->isPrivMsgGet() && !line->isSelfMsgGet()))) {
            QString title;
            if (buffer->isChannelGet() || buffer->isServerGet()) {
                title = tr("New highlight in %1 from %2").arg(buffer->short_nameGet().toPlain()).arg(line->colorlessNicknameGet());
            } else {
                title = tr("New message from %1").arg(buffer->short_nameGet().toPlain());
            }

#if defined(__linux) && !defined(__ANDROID_API__)
            QString serviceName = QStringLiteral("org.freedesktop.Notifications");
            QDBusConnection dbus = QDBusConnection::sessionBus();
            QDBusConnectionInterface* connectionInterface = dbus.interface();
            bool isRegistered = connectionInterface->isServiceRegistered(serviceName);

            if (isRegistered) {
                QDBusInterface notifications(serviceName, QStringLiteral("/org/freedesktop/Notifications"), serviceName, dbus);
                auto reply = notifications.call(
                    QStringLiteral("Notify"), QStringLiteral("Lith"), 0U, QStringLiteral("org.LithApp.Lith"), title,
                    line->colorlessTextGet(), QStringList {}, QVariantMap {}, -1
                );
            } else {
                static QIcon appIcon(QStringLiteral(":/icon.png"));
                static auto* icon = new QSystemTrayIcon(appIcon);
                icon->show();

                icon->showMessage(title, line->colorlessTextGet(), appIcon);
            }
#else
            static QIcon appIcon(":/icon.png");
            static QSystemTrayIcon* icon = new QSystemTrayIcon(appIcon);
            icon->show();

            icon->showMessage(title, line->colorlessTextGet(), appIcon);
#endif  // __linux
        }
    }
}

void Lith::_nicklist(const WeeChatProtocol::HData& hda) {
    Buffer* previousBuffer = nullptr;
    for (const auto& i : hda.data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto* buffer = getBuffer(bufPtr);
        if (!buffer) {
            continue;
        }
        if (buffer != previousBuffer) {
            buffer->clearNicks();
        }
        previousBuffer = buffer;
        auto* nick = new Nick(buffer);
        for (auto [key, value] : i.objects.asKeyValueRange()) {
            nick->setProperty(qPrintable(key), value);
        }
        buffer->addNick(nickPtr, nick);
    }
}

void Lith::_nicklist_diff(const WeeChatProtocol::HData& hda) {
    for (const auto& i : hda.data) {
        // buffer - nicklist_item
        auto bufPtr = i.pointers.first();
        auto nickPtr = i.pointers.last();
        auto* buffer = getBuffer(bufPtr);
        if (!buffer) {
            continue;
        }
        auto op = qvariant_cast<char>(i.objects[QStringLiteral("_diff")]);
        switch (op) {
            case '+': {
                auto* nick = new Nick(buffer);
                for (auto [key, value] : i.objects.asKeyValueRange()) {
                    if (key == QStringLiteral("_diff")) {
                        continue;
                    }
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
                auto* nick = buffer->getNick(nickPtr);
                if (!nick) {
                    break;
                }
                for (auto [key, value] : i.objects.asKeyValueRange()) {
                    if (key == QStringLiteral("_diff")) {
                        continue;
                    }
                    nick->setProperty(qPrintable(key), value);
                }
                break;
            }
            default:
                break;
        }
    }
}

void Lith::_pong(const FormattedString& str) {
    emit pongReceived(str.toLongLong());
}

void Lith::addBuffer(weechat_pointer_t ptr, Buffer* b) {
    m_bufferMap[ptr] = b;
    m_buffers->append(b);
    auto lastOpenBuffer = settingsGet()->lastOpenBufferGet();
    if (m_buffers->count() == 1 && lastOpenBuffer < 0) {
        emit selectedBufferChanged();
    } else if (lastOpenBuffer == m_buffers->count() - 1) {
        selectedBufferIndexSet(lastOpenBuffer);
    }
}

void Lith::removeBuffer(weechat_pointer_t ptr) {
    if (m_bufferMap.contains(ptr)) {
        auto buf = m_bufferMap[ptr];
        if (selectedBuffer() == buf) {
            selectedBufferIndexSet(selectedBufferIndex() - 1);
        }
        m_bufferMap.remove(ptr);
        m_buffers->removeItem(buf);
    }
}

Buffer* Lith::getBuffer(weechat_pointer_t ptr) {
    return m_bufferMap.value(ptr, nullptr);
}

void Lith::addLine(weechat_pointer_t bufPtr, weechat_pointer_t linePtr, BufferLine* line) {
    m_lineMap[bufPtr][linePtr] = line;
}

BufferLine* Lith::getLine(weechat_pointer_t bufPtr, weechat_pointer_t linePtr) {
    if (m_lineMap.contains(bufPtr)) {
        return m_lineMap[bufPtr].value(linePtr, nullptr);
    }
    return nullptr;
}

const BufferLine* Lith::getLine(weechat_pointer_t bufPtr, weechat_pointer_t linePtr) const {
    if (m_lineMap.contains(bufPtr)) {
        return m_lineMap.value(bufPtr).value(linePtr, nullptr);
    }
    return nullptr;
}

void Lith::addHotlist(weechat_pointer_t ptr, HotListItem* hotlist) {
    if (m_hotList.contains(ptr)) {
        // TODO
        qCritical() << QStringLiteral("Hotlist with ptr") << QStringLiteral("%1").arg(ptr, 8, 16, QLatin1Char('0'))
                    << QStringLiteral("already exists");
    }
    m_hotList[ptr] = hotlist;
}

HotListItem* Lith::getHotlist(weechat_pointer_t ptr) {
    return m_hotList.value(ptr, {});
}

const Buffer* Lith::getBuffer(weechat_pointer_t ptr) const {
    return m_bufferMap.value(ptr, nullptr);
}

const HotListItem* Lith::getHotlist(weechat_pointer_t ptr) const {
    return m_hotList.value(ptr, nullptr);
}

ProxyBufferList::ProxyBufferList(Lith* parent, QAbstractListModel* parentModel)
    : QSortFilterProxyModel(parent) {
    setSourceModel(parentModel);
    setFilterRole(Qt::UserRole);
    sort(0);
    connect(this, &ProxyBufferList::filterWordChanged, [this] { setFilterFixedString(filterWordGet()); });
    connect(Lith::settingsGet(), &Settings::bufferListShowsOnlyBuffersWithNewMessagesChanged, this, [this]() {
        showOnlyBuffersWithNewMessagesSet(Lith::settingsGet()->bufferListShowsOnlyBuffersWithNewMessagesGet());
        invalidateFilter();
    });
    connect(Lith::settingsGet(), &Settings::bufferListGroupingByServerChanged, this, [this]() { invalidate(); });
    connect(Lith::settingsGet(), &Settings::bufferListAlphabeticalSortingChanged, this, [this]() { invalidate(); });
    connect(parent, &Lith::hotlistUpdated, this, [this] {
        if (showOnlyBuffersWithNewMessagesGet()) {
            invalidateFilter();
        }
    });
    showOnlyBuffersWithNewMessagesSet(Lith::settingsGet()->bufferListShowsOnlyBuffersWithNewMessagesGet());
}

bool ProxyBufferList::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    // If there are no conditions, just bail early
    if (filterWordGet().isEmpty() && !showOnlyBuffersWithNewMessagesGet()) {
        return true;
    }

    auto index = sourceModel()->index(source_row, 0, source_parent);
    auto v = sourceModel()->data(index, Qt::UserRole);
    auto* b = qvariant_cast<Buffer*>(v);
    if (b) {
        if (filterWordGet().isEmpty()) {
            if (showOnlyBuffersWithNewMessagesGet()) {
                return b->totalUnreadMessagesGet() > 0;
            }
            return true;
        } else {
            if (b->isServerGet()) {
                return b->short_nameGet().toLower().contains(filterWordGet().toLower());
            } else {
                return b->nameGet().toLower().contains(filterWordGet().toLower());
            }
        }
    }
    return false;
}

bool ProxyBufferList::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const {
    // auto index = sourceModel()->index(source_left, 0, QModelIndex());
    const auto* l = sourceModel()->data(source_left, Qt::UserRole).value<Buffer*>();
    const auto* r = sourceModel()->data(source_right, Qt::UserRole).value<Buffer*>();

    const QString lName = (l ? l->isRelayGet() ? l->nameGet() : l->short_nameGet() : FormattedString {}).toPlain().toLower();
    const QString rName = (r ? r->isRelayGet() ? r->nameGet() : r->short_nameGet() : FormattedString {}).toPlain().toLower();

    if (Lith::settingsGet()->instance()->bufferListGroupingByServerGet()) {
        auto serverResult = QString::localeAwareCompare(l->serverGet(), r->serverGet());
        if (serverResult < 0) {
            return true;
        } else if (serverResult > 0) {
            return false;
        }

        if (l->isServerGet() && !r->isServerGet()) {
            return true;
        }
        if (!l->isServerGet() && r->isServerGet()) {
            return false;
        }

        if (Lith::settingsGet()->instance()->bufferListAlphabeticalSortingGet()) {

            auto nameResult = QString::localeAwareCompare(lName, rName);
            if (nameResult < 0) {
                return true;
            } else if (nameResult > 0) {
                return false;
            }

            auto numberResult = l->numberGet() - r->numberGet();
            if (numberResult < 0) {
                return true;
            } else if (numberResult > 0) {
                return false;
            }

        } else {
            auto numberResult = l->numberGet() - r->numberGet();
            if (numberResult < 0) {
                return true;
            } else if (numberResult > 0) {
                return false;
            }

            auto nameResult = QString::localeAwareCompare(lName, rName);
            if (nameResult < 0) {
                return true;
            } else if (nameResult > 0) {
                return false;
            }
        }

        return false;
    } else if (Lith::settingsGet()->instance()->bufferListAlphabeticalSortingGet()) {
        auto nameResult = QString::localeAwareCompare(lName, rName);
        if (nameResult < 0) {
            return true;
        } else if (nameResult > 0) {
            return false;
        }

        auto numberResult = l->numberGet() - r->numberGet();
        if (numberResult < 0) {
            return true;
        } else if (numberResult > 0) {
            return false;
        }

        return false;
    } else {
        return source_left.row() < source_right.row();
    }
}
