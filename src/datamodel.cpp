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
#include <QXmlStreamReader>
#include <QDomDocument>

static const QMap<QString, QString> lightModeColors {
    { "default",       "black" },
    { "black",         "black" },
    { "dark gray",     "#444444" },
    { "dark red",      "#880000" },
    { "light red",     "#ff4444" },
    { "dark green",    "#008800" },
    { "light green",   "#33cc33" },
    { "brown",         "#d2691e" },
    { "yellow",        "#dddd00" },
    { "dark blue",     "#000088" },
    { "light blue",    "#3333dd" },
    { "dark magenta",  "#660066" },
    { "light magenta", "#ff44ff" },
    { "dark cyan",     "#006666" },
    { "light cyan",    "#22aaaa" },
    { "gray",          "#aaaaaa" },
    { "white",         "#ffffff" }
};

static const QMap<QString, QString> darkModeColors {
    { "default",       "white" },
    { "black",         "black" },
    { "dark gray",     "#444444" },
    { "dark red",      "#880000" },
    { "light red",     "#ff4444" },
    { "dark green",    "#33dd33" },
    { "light green",   "#55ff55" },
    { "brown",         "#d2691e" },
    { "yellow",        "#ffff00" },
    { "dark blue",     "#4444ff" },
    { "light blue",    "#9999ff" },
    { "dark magenta",  "#ee44ee" },
    { "light magenta", "#ff88ff" },
    { "dark cyan",     "#22aaaa" },
    { "light cyan",    "#44dddd" },
    { "gray",          "#aaaaaa" },
    { "white",         "#ffffff" }
};


Buffer::Buffer(QObject *parent, pointer_t pointer)
    : QObject(parent)
    , m_lines(QmlObjectList::create<BufferLine>(this))
    , m_nicks(QmlObjectList::create<Nick>(this))
    , m_ptr(pointer)
{

}

Buffer::~Buffer() {
    m_nicks->clear();
    m_lines->clear();
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
            if (n->prefixGet().trimmed().isEmpty() || n->prefixGet() == "<html><body> </body></html>")
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
            if (n->prefixGet() == "<html><body>+</body></html>")
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
            if (n->prefixGet() == "<html><body>@</body></html>")
                total++;
        }
    }
    return total;
}

QStringList Buffer::local_variables_stringListGet() const {
    QStringList ret;
    for (auto i : m_local_variables.keys()) {
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

bool Buffer::input(const QString &data) {
    if (Lith::instance()->statusGet() == Lith::CONNECTED) {
        bool success = false;
        QMetaObject::invokeMethod(Lith::instance()->weechat(), "input", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, success), Q_ARG(pointer_t, m_ptr), Q_ARG(const QString&, data));
        return success;
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
    connect(this, &BufferLine::messageChanged, this, &BufferLine::onMessageChanged);
}

BufferLine::~BufferLine() {
}

QString BufferLine::prefixGet() const {
    QString ret;
    if (nickAttributeGet().count() > 0) {
        ret += "<font color=\"" + nickAttributeColorGet() + "\">" + nickAttributeGet() + "</color>";
    }
    if (nickGet().count() > 0) {
        ret += "<font color=\"" + nickColorGet() + "\">" + nickGet() + "</color>";
    }
    return ret;
}

void BufferLine::prefixSet(const QString &o) {
    QXmlStreamReader xml(o);
    QStringList colors;
    QStringList parts;
    while (!xml.atEnd()) {
        xml.readNextStartElement();
        if (xml.attributes().count() >= 1 && xml.attributes().first().name() == "color") {
            colors.append(xml.attributes().first().value().toString());
            parts.append(xml.readElementText(QXmlStreamReader::IncludeChildElements));
        }
    }
    bool changed = false;
    if (parts.count() == 2 && colors.count() == 2) {
        if (m_nickAttr != parts[0] || m_nickAttrColor != colors[0]) {
            changed = true;
            m_nickAttr = parts[0];
            m_nickAttrColor = colors[0];
        }
        if (m_nick != parts[1] || m_nickColor != colors[1]) {
            changed = true;
            m_nick = parts[1];
            m_nickColor = colors[1];
        }
    }
    else if (parts.count() >= 1 && colors.count() >= 1){
        if (m_nick != parts[0] || m_nickColor != colors[0]) {
            changed = true;
            m_nick = parts[0];
            m_nickColor = colors[0];
        }
    }
    else if (parts.count() == 1) {
        if (m_nick != parts[0] || m_nickColor != "default") {
            changed = true;
            m_nick = parts[0];
            m_nickColor = "default";
        }
    }
    if (changed)
        emit prefixChanged();
}

QString BufferLine::nickAttributeGet() const {
    return m_nickAttr;
}

QString BufferLine::nickAttributeColorGet() const {
    if (lightModeColors.contains(m_nickAttrColor))
        return lightModeColors[m_nickAttrColor];
    return m_nickAttrColor;
}

QString BufferLine::nickGet() const {
    return m_nick;
}

QString BufferLine::nickColorGet() const {
    if (lightModeColors.contains(m_nickColor))
        return lightModeColors[m_nickColor];
    return m_nickColor;
}

QString BufferLine::messageGet() const {
    return m_message;
}

void BufferLine::messageSet(const QString &o) {
    // Originally: QRegExp re(R"(((?:(?:https?|ftp|file):\/\/|www\.|ftp\.)(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.]*\)|[-A-Z0-9+&@#\/%=~_|$?!:,.])*(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.]*\)|[A-Z0-9+&@#\/%=~_|$])))", Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
    // ; was added to handle &amp; escapes right
    QRegExp re(R"(((?:(?:https?|ftp|file):\/\/|www\.|ftp\.)(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.;]*\)|[-A-Z0-9+&@#\/%=~_|$?!:,.;])*(?:\([-A-Z0-9+&@#\/%=~_|$?!:,.;]*\)|[A-Z0-9+&@#\/%=~_|$;])))", Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
    auto copy = o;
    if (re.indexIn(copy) >= 0) {
        auto url = re.cap();
        copy.replace(re, "<a href=\""+url+"\">"+url+"</a>");
    }

    QDomDocument doc;
    QString error;
    doc.setContent(copy, &error);
    auto fontList = doc.elementsByTagName("font");
    for (int i = 0; i < fontList.size(); i++) {
        auto elem = fontList.at(i).toElement();
        auto oldColor = elem.attribute("color");
        elem.setAttribute("color", lightModeColors[oldColor]);
    }
    if (!error.isEmpty()) {
        qWarning() << "URL parsing error" << error << "when handling message:" << o;
        qWarning() << "Offending result:" << copy;
    }
    else {
        copy = doc.toString(-1);
    }

    if (copy != m_message) {
        m_message = copy;
        emit messageChanged();
    }
}

bool BufferLine::isSelfMsgGet() {
    return m_tags_array.contains("self_msg");
}

bool BufferLine::isPrivMsgGet() {
    return m_tags_array.contains("irc_privmsg");
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

Nick::~Nick() {
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
