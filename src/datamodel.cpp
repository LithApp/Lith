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

#include <QUrl>
#include <QApplication>
#include <QTextDocumentFragment>

static const QMap<QString, QString> lightModeColors {
    { "<font color=\"default\">", "<font color=\"black\">" },
    { "<font color=\"black\">", "<font color=\"black\">" },
    { "<font color=\"dark gray\">", "<font color=\"#444444\">" },
    { "<font color=\"dark red\">", "<font color=\"#880000\">" },
    { "<font color=\"light red\">", "<font color=\"#ff4444\">" },
    { "<font color=\"dark green\">", "<font color=\"#008800\">" },
    { "<font color=\"light green\">", "<font color=\"#33cc33\">" },
    { "<font color=\"brown\">", "<font color=\"#d2691e\">" },
    { "<font color=\"yellow\">", "<font color=\"#dddd00\">" },
    { "<font color=\"dark blue\">", "<font color=\"#000088\">" },
    { "<font color=\"light blue\">", "<font color=\"#3333dd\">" },
    { "<font color=\"dark magenta\">", "<font color=\"#660066\">" },
    { "<font color=\"light magenta\">", "<font color=\"#ff44ff\">" },
    { "<font color=\"dark cyan\">", "<font color=\"#006666\">" },
    { "<font color=\"light cyan\">", "<font color=\"#22aaaa\">" },
    { "<font color=\"gray\">", "<font color=\"#aaaaaa\">" },
    { "<font color=\"white\">", "<font color=\"#ffffff\">" }
};

static const QMap<QString, QString> darkModeColors {
    { "<font color=\"default\">", "<font color=\"white\">" },
    { "<font color=\"black\">", "<font color=\"black\">" },
    { "<font color=\"dark gray\">", "<font color=\"#444444\">" },
    { "<font color=\"dark red\">", "<font color=\"#880000\">" },
    { "<font color=\"light red\">", "<font color=\"#ff4444\">" },
    { "<font color=\"dark green\">", "<font color=\"#33dd33\">" },
    { "<font color=\"light green\">", "<font color=\"#55ff55\">" },
    { "<font color=\"brown\">", "<font color=\"#d2691e\">" },
    { "<font color=\"yellow\">", "<font color=\"#ffff00\">" },
    { "<font color=\"dark blue\">", "<font color=\"#4444ff\">" },
    { "<font color=\"light blue\">", "<font color=\"#9999ff\">" },
    { "<font color=\"dark magenta\">", "<font color=\"#ee44ee\">" },
    { "<font color=\"light magenta\">", "<font color=\"#ff88ff\">" },
    { "<font color=\"dark cyan\">", "<font color=\"#22aaaa\">" },
    { "<font color=\"light cyan\">", "<font color=\"#44dddd\">" },
    { "<font color=\"gray\">", "<font color=\"#aaaaaa\">" },
    { "<font color=\"white\">", "<font color=\"#ffffff\">" }
};


Buffer::Buffer(QObject *parent, pointer_t pointer)
    : QObject(parent)
    , m_lines(QmlObjectList::create<BufferLine>(this))
    , m_nicks(QmlObjectList::create<Nick>(this))
    , m_ptr(pointer)
{

}

void Buffer::prependLine(BufferLine *line) {
    m_lines->prepend(line);
}

void Buffer::appendLine(BufferLine *line) {
    m_lines->append(line);
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
}

void Buffer::removeNick(pointer_t ptr) {
    for (int i = 0; i < m_nicks->count(); i++) {
        auto n = m_nicks->get<Nick>(i);
        if (n && n->ptrGet() == ptr) {
            m_nicks->removeRow(i);
            break;
        }
    }
}

void Buffer::clearNicks() {
    for (int i = 0; i < m_nicks->count(); i++) {
        auto n = m_nicks->get<Nick>(i);
        if (n)
            n->deleteLater();
    }
    m_nicks->clear();
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

void Buffer::input(const QString &data) {
    QMetaObject::invokeMethod(Lith::instance()->weechat(), "input", Q_ARG(pointer_t, m_ptr), Q_ARG(const QString&, data));
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

BufferLine::BufferLine(Buffer *parent)
    : QObject(parent)
{
    connect(this, &BufferLine::messageChanged, this, &BufferLine::onMessageChanged);
}

QString BufferLine::prefixGet() const {
    auto ret = m_prefix;
    for (auto i : darkModeColors.keys()) {
        ret.replace(i, darkModeColors[i]);
    }
    return ret;
}

void BufferLine::prefixSet(const QString &o) {
    if (m_prefix != o) {
        m_prefix = o;
        emit prefixChanged();
    }
}

QString BufferLine::messageGet() const {
    auto ret = m_message;
    for (auto i : darkModeColors.keys()) {
        ret.replace(i, darkModeColors[i]);
    }
    return ret;
}

void BufferLine::messageSet(const QString &o) {
    QRegExp re(R"(((?:(?:https?|ftp|file):\/\/|www\.|ftp\.)(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.]*\)|[-A-Z0-9+&@#\/%=~_|$?!:,.])*(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.]*\)|[A-Z0-9+&@#\/%=~_|$])))", Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
    auto copy = o;
    copy.replace(re, "<a href=\"\\1\">\\1</a>");
    if (copy != m_message) {
        m_message = copy;
        emit messageChanged();
    }
}

bool BufferLine::isPrivMsg() {
    return m_tags_array.contains("irc_privmsg");
}

QString BufferLine::colorlessNicknameGet() {
    auto nickStripped = QTextDocumentFragment::fromHtml(m_prefix).toPlainText();
    return nickStripped;
}

QString BufferLine::colorlessTextGet() {
    auto messageStripped = QTextDocumentFragment::fromHtml(m_message).toPlainText();
    return messageStripped;
}

QObject *BufferLine::bufferGet() {
    return parent();
}

QList<QObject *> BufferLine::segments() {
    return m_segments;
}

void BufferLine::onMessageChanged() {
    QRegExp re(R"((?:(?:https?|ftp|file):\/\/|www\.|ftp\.)(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.]*\)|[-A-Z0-9+&@#\/%=~_|$?!:,.])*(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.]*\)|[A-Z0-9+&@#\/%=~_|$]))", Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
    int lastIdx = 0;
    int idx = -1;
    if (lastIdx < messageGet().length() && (idx = re.indexIn(messageGet(), lastIdx)) >= 0) {
        int length = idx - lastIdx;
        auto leftPart = messageGet().mid(lastIdx, length).trimmed();
        auto rightPart = re.cap().trimmed();
        if (!leftPart.isEmpty())
            m_segments.append(new BufferLineSegment(this, leftPart));
        if (!rightPart.isEmpty())
            m_segments.append(new BufferLineSegment(this, rightPart, BufferLineSegment::LINK));

        lastIdx = idx + re.matchedLength();
    }

    if (lastIdx < messageGet().length()) {
        auto rest = messageGet().mid(lastIdx).trimmed();
        if (!rest.isEmpty())
            m_segments.append(new BufferLineSegment(this, rest));
    }

    emit segmentsChanged();
}

Nick::Nick(Buffer *parent)
    : QObject(parent)
{

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
    if (bufferGet() && countGet().count() >= 3) {
        bufferGet()->hotMessagesSet(countGet()[2]);
        bufferGet()->unreadMessagesSet(countGet()[1]);
    }
}

BufferLineSegment::BufferLineSegment(BufferLine *parent, const QString &text, BufferLineSegment::Type type)
    : QObject(parent)
    , m_type(type)
    , m_plainText(text)
{
    connect(Lith::instance()->settingsGet(), &Settings::shortenLongUrlsThresholdChanged, this, &BufferLineSegment::summaryChanged);
    auto url = QUrl(plainTextGet());
    if (url.scheme().startsWith("http")) {
        auto extension = url.fileName().split(".").last().toLower();
        auto host = url.host();

        if (QStringList{"png", "jpg", "gif"}.indexOf(extension) != -1)
            m_type = IMAGE;
        else if (QStringList{"avi", "mov", "mp4", "webm"}.indexOf(extension) != -1)
            m_type = VIDEO;
        else if (host.contains("youtube.com")) {
            QRegExp re("[?]v[=]([0-9a-zA-Z-_]+)");
            if (re.indexIn(plainTextGet()) != -1) {
                m_embedUrl = "https://www.youtube.com/embed/" + re.cap(1);
                m_type = EMBED;
            }
            else
                m_type = LINK;
        }
        else
            m_type = LINK;
        // youtube: "https://www.youtube.com/embed/IDidIDidIDi"
    }
}
QString BufferLineSegment::summaryGet()
{
    const auto threshold = Lith::instance()->settingsGet()->shortenLongUrlsThresholdGet();
    if (plainTextGet().size() < threshold) {
        return plainTextGet();
    }
    auto url = QUrl(plainTextGet());
    auto scheme = url.scheme();
    auto host = url.host();
    auto file = url.fileName();
    auto query = url.query();
    auto path = url.path();
    // If we only have a hostname, we'll use it as is.
    if (path.isEmpty() || path == "/") {
        return plainTextGet();
    }

    // We'll show always show the host and the scheme.
    const auto hostPrefix = scheme + "://" + host + "/";
    const auto ellipsis = "\u2026";

    // The threshold is so small that it doesn't even accomodate the hostPrefix. We'll just put the hostPrefix and
    // ellipsis...
    if (hostPrefix.length() > threshold) {
        return hostPrefix + ellipsis;
    }

    // This is a "nice" url with just a hostname and then one path fragment. We'll let these slide, because these tend
    // to look nice even if they're long. Something like https://host.domain/file.extension
    if (path == "/" + file && !url.hasQuery()) {
        return plainTextGet();
    }

    // Otherwise it's a weird link with multiple path fragments and queries and stuff. We'll just use the host and 10
    // characters of the path.
    const auto maxCharsToAppend = threshold - hostPrefix.length();
    return hostPrefix + ellipsis + plainTextGet().right(maxCharsToAppend - 1);
}
