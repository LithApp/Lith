// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
// Copyright (C) 2020 Jakub Mach
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

#include "datamodel.h"

#include "weechat.h"
#include "lith.h"
#include "windowhelper.h"

#include <QUrl>
#include <QApplication>
#include <QTextDocumentFragment>
#include <QXmlStreamReader>
#include <QDomDocument>

Buffer::Buffer(Lith *parent, pointer_t pointer)
    : QObject(parent)
    , m_lines(QmlObjectList::create<BufferLine>(this))
    , m_nicks(QmlObjectList::create<Nick>(this))
    , m_proxyLinesFiltered(new MessageFilterList(this, m_lines))
    , m_ptr(pointer)
{

}

Buffer::~Buffer() {
    m_nicks->clear();
    m_lines->clear();
}

Lith *Buffer::lith() {
    return qobject_cast<Lith*>(parent());
}

void Buffer::prependLine(BufferLine *line) {
    m_lines->prepend(line);
}

void Buffer::appendLine(BufferLine *line) {
    m_lines->append(line);
}

FormattedString Buffer::titleGet() const {
    return m_title;
}

void Buffer::titleSet(const FormattedString &o) {
    if (m_title != o) {
        m_title = o;
        emit titleChanged();
    }
}

bool Buffer::isAfterInitialFetch() {
    return m_afterInitialFetch;
}

QmlObjectList *Buffer::lines() {
    return m_lines;
}

QmlObjectList *Buffer::nicks() {
    return m_nicks;
}

Nick *Buffer::getNick(pointer_t ptr) {
    for (int i = 0; i < m_nicks->count(); i++) {
        auto n = m_nicks->get<Nick>(i);
        if (n && n->ptrGet() == ptr) {
            return n;
        }
    }
    return nullptr;

    /*
    auto nick = new Nick(this);
    m_nicks.append(nick);
    nick->ptrSet(ptr);
    emit nicksChanged();
    return nick;
    */
}

void Buffer::addNick(pointer_t ptr, Nick *nick) {
    nick->ptrSet(ptr);
    m_nicks->append(nick);
    emit nicksChanged();
}

void Buffer::removeNick(pointer_t ptr) {
    for (int i = 0; i < m_nicks->count(); i++) {
        auto n = m_nicks->get<Nick>(i);
        if (n && n->ptrGet() == ptr) {
            m_nicks->removeRow(i);
            emit nicksChanged();
            break;
        }
    }
}

void Buffer::clearNicks() {
    m_nicks->clear();
    emit nicksChanged();
}

QStringList Buffer::getVisibleNicks() {
    QStringList result;
    for (int i = 0; i < m_nicks->count(); i++) {
        auto nick = m_nicks->get<Nick>(i);
        if (nick && nick->visibleGet() && nick->levelGet() == 0) {
            result.append(nick->nameGet());
        }
    }
    return result;
}

int Buffer::normalsGet() const {
    int total = 0;
    for (int i = 0; i < m_nicks->count(); i++) {
        auto n = m_nicks->get<Nick>(i);
        if (n && n->visibleGet() && n->levelGet() == 0) {
            if (n->prefixGet().trimmed().isEmpty() || n->prefixGet() == "<html><body><span style='white-space: pre-wrap;'> </span></body></html>")
                total++;
        }
    }
    return total;
}

int Buffer::voicesGet() const {
    int total = 0;
    for (int i = 0; i < m_nicks->count(); i++) {
        auto n = m_nicks->get<Nick>(i);
        if (n && n->visibleGet() && n->levelGet() == 0) {
            if (n->prefixGet() == "<html><body><span style='white-space: pre-wrap;'>+</span></body></html>")
                total++;
        }
    }
    return total;
}

int Buffer::opsGet() const {
    int total = 0;
    for (int i = 0; i < m_nicks->count(); i++) {
        auto n = m_nicks->get<Nick>(i);
        if (n && n->visibleGet() && n->levelGet() == 0) {
            if (n->prefixGet() == "<html><body><span style='white-space: pre-wrap;'>@</span></body></html>")
                total++;
        }
    }
    return total;
}

QStringList Buffer::local_variables_stringListGet() const {
    QStringList ret;
    for (auto &i : m_local_variables.keys()) {
        ret.append(QString("%1: %2").arg(i).arg(m_local_variables[i]));
    }
    return ret;
}

bool Buffer::isServerGet() const {
    return m_local_variables.contains("type") && m_local_variables["type"] == "server";
}

bool Buffer::isChannelGet() const {
    return m_local_variables.contains("type") && m_local_variables["type"] == "channel";
}

bool Buffer::isPrivateGet() const {
    return m_local_variables.contains("type") && m_local_variables["type"] == "private";
}

MessageFilterList *Buffer::lines_filtered() {
    return m_proxyLinesFiltered;
}

bool Buffer::input(const QString &data) {
    if (Lith::instance()->statusGet() == Lith::CONNECTED) {
        bool success = false;
        QList<bool> success_list;

        auto data_split = data.split(QRegularExpression("\n|\r\n|\r"));

        for (auto &data_single_line : data_split) {
            QMetaObject::invokeMethod(Lith::instance()->weechat(), "input", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, success), Q_ARG(pointer_t, m_ptr), Q_ARG(QString, data_single_line));
            success_list.append(success);
        }
        return !success_list.contains(false);
    }
    return false;
    //Lith::instance()->weechat()->input(m_ptr, data);
}

void Buffer::fetchMoreLines() {
    m_afterInitialFetch = true;
    if (m_lines->count() >= m_lastRequestedCount) {
        QMetaObject::invokeMethod(Lith::instance()->weechat(), "fetchLines", Q_ARG(pointer_t, m_ptr), Q_ARG(int, m_lines->count() + 25));
        //Lith::instance()->weechat()->fetchLines(m_ptr, m_lines->count() + 25);
        m_lastRequestedCount = m_lines->count() + 25;
    }
}

void Buffer::clearHotlist() {
    input("/buffer set hotlist -1");
    unreadMessagesSet(0);
    hotMessagesSet(0);
}

BufferLine::BufferLine(Buffer *parent)
    : QObject(parent)
{
    // connections to changed signals are required because FormattedString isn't directly connected to signals that can directly modify it
    // having it here results in fewer connections and that means lower cpu load when the signals are fired
    connect(Lith::instance()->settingsGet(), &Settings::shortenLongUrlsThresholdChanged, this, &BufferLine::messageChanged);
    connect(Lith::instance()->settingsGet(), &Settings::shortenLongUrlsChanged, this, &BufferLine::messageChanged);
    connect(Lith::instance()->windowHelperGet(), &WindowHelper::themeChanged, this, &BufferLine::messageChanged);
    connect(Lith::instance()->windowHelperGet(), &WindowHelper::themeChanged, this, &BufferLine::prefixChanged);
}

BufferLine::~BufferLine() {
}

Buffer *BufferLine::buffer() {
    return qobject_cast<Buffer*>(parent());
}

Lith *BufferLine::lith() {
    if (buffer())
        return buffer()->lith();
    return nullptr;
}

FormattedString BufferLine::prefixGet() const {
    return m_prefix;
}

void BufferLine::prefixSet(const FormattedString &o) {
    if (m_prefix != o) {
        m_prefix = o;
        // TODO this is probably wrong
        if (m_prefix.toPlain().startsWith("@") || m_prefix.toPlain().startsWith("+")) {
            m_nick = m_prefix.toPlain().mid(1);
        }
        else {
            m_nick = m_prefix.toPlain();
        }
        emit prefixChanged();
    }
}

QString BufferLine::nickGet() const {
    return m_nick;
}

FormattedString BufferLine::messageGet() const {
    return m_message;
}

void BufferLine::messageSet(const FormattedString &o) {
    if (m_message != o) {
        m_message = o;
        emit messageChanged();
    }
}

bool BufferLine::isSelfMsgGet() {
    return m_tags_array.contains("self_msg");
}

QColor BufferLine::nickColorGet() const {
    // TODO this is suspicious at best
    if (m_prefix.count() > 2)
        return m_prefix.at(2).foreground.toQColor(Lith::instance()->windowHelperGet()->inverseTheme());
    return QColor();
}

bool BufferLine::isPrivMsgGet() {
    return m_tags_array.contains("irc_privmsg");
}

bool BufferLine::isJoinPartQuitMsgGet() {
    return m_tags_array.contains("irc_quit") || m_tags_array.contains("irc_join") || m_tags_array.contains("irc_part");
}

QString BufferLine::colorlessNicknameGet() {
    return m_nick;
}

QString BufferLine::colorlessTextGet() {
    auto messageStripped = QTextDocumentFragment::fromHtml(m_message).toPlainText();
    return messageStripped;
}

QObject *BufferLine::bufferGet() {
    return parent();
}

Nick::Nick(Buffer *parent)
    : QObject(parent)
{

}

Nick::~Nick() {
}

QString Nick::colorlessName() const
{
    return m_name.toPlain();
}

HotListItem::HotListItem(QObject *parent)
    : QObject(parent)
{
    connect(this, &HotListItem::countChanged, this, &HotListItem::onCountChanged);
    connect(this, &HotListItem::bufferChanged, this, &HotListItem::onCountChanged);
}

Buffer *HotListItem::bufferGet() {
    return m_buffer;
}

void HotListItem::bufferSet(Buffer *o) {
    if (m_buffer != o) {
        m_buffer = o;
        emit bufferChanged();
    }
}

void HotListItem::onCountChanged() {
    if (bufferGet()) {
        if (countGet().count() >= 3) {
            bufferGet()->hotMessagesSet(countGet()[2]);
            bufferGet()->unreadMessagesSet(countGet()[1]);
        }
        else if (countGet().count() >= 2) {
            bufferGet()->hotMessagesSet(0);
            bufferGet()->unreadMessagesSet(countGet()[1]);
        }
        else {
            bufferGet()->hotMessagesSet(0);
            bufferGet()->unreadMessagesSet(0);
        }
    }
}
