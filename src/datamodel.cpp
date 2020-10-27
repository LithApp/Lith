#include "datamodel.h"

#include "weechat.h"
#include "lith.h"

#include <QUrl>
#include <QApplication>
#include <QAbstractEventDispatcher>

bool W::parse(QDataStream &s, W::Char &r) {
    s.readRawData(&r.d, 1);
    return true;
}

bool W::parse(QDataStream &s, W::Integer &r) {
    s.setByteOrder(QDataStream::BigEndian);
    s >> r.d;
    return true;
}

bool W::parse(QDataStream &s, W::LongInteger &r) {
    quint8 length;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    r.d = buf.toLongLong();
    return true;
}

static const QMap<int, QString> weechatColors {
    { 0, "" },
    { 1, "black" },
    { 2, "dark gray" },
    { 3, "#8b0000" },
    { 4, "#ffcccb" },
    { 5, "dark green" },
    { 6, "light green" },
    { 7, "brown" },
    { 8, "yellow" },
    { 9, "#00008b" },
    { 10, "#add8e6" },
    { 11, "#8b008b" },
    { 12, "#ff80ff" },
    { 13, "#008b8b" },
    { 14, "#e0ffff" },
    { 15, "gray" },
    { 16, "white" }
};

bool W::parse(QDataStream &s, W::String &r) {
    uint32_t len;
    r.d.clear();
    s >> len;
    if (len == uint32_t(-1))
        r.d = QString();
    else if (len == 0)
        r.d = "";
    else if (len > 0) {
        /*
        int i;
        while ((i = buf.indexOf((char) 0x1a)) >= 0) {
            buf.replace(i, 2, "");
        }
        while ((i = buf.indexOf((char) 0x1b)) >= 0) {
            buf.replace(i, 2, "");
        }
        while ((i = buf.indexOf((char) 0x1c)) >= 0) {
            buf.replace(i, 1, "");
        }
        while ((i = buf.indexOf((char) 0x19)) >= 0) {
            if (buf[i+1] == 'F')
                buf.replace(i, 4, "");
            else
                buf.replace(i, 3, "");
        }
        r.d = buf;
        */
        QByteArray buf(len + 1, 0);
        s.readRawData(buf.data(), len);
        r.d.clear();
        bool foreground = false;
        bool background = false;
        bool bold = false;
        bool reverse = false;
        bool italic = false;
        bool underline = false;
        bool keep = false;
        auto endColors = [&r, &foreground, &background]() {
            if (foreground) {
                foreground = false;
                r.d += "</font>";
            }
            if (background) {
                background = false;
                r.d += "</span>";
            }
        };
        auto endAttrs = [&r, &bold, &reverse, &italic, &underline, &keep]() {
            if (bold) {
                bold = false;
                r.d += "</b>";
            }
            if (reverse) {
                reverse = false;
                // TODO
            }
            if (italic) {
                italic = false;
                r.d += "</i>";
            }
            if (underline) {
                underline = false;
                r.d += "</u>";
            }
            if (keep) {
                // TODO
            }
        };
        auto loadAttr = [&r, &bold, &reverse, &italic, &underline, &keep](QByteArray::iterator &it) {
            while (true) {
                switch(*it) {
                case '*':
                    if (bold)
                        break;
                    bold = true;
                    r.d += "<b>";
                    break;
                case '!':
                    if (reverse)
                        break;
                    reverse = true;
                    // TODO
                    break;
                case '/':
                    if (italic)
                        break;
                    italic = true;
                    r.d += "<i>";
                    break;
                case '_':
                    if (underline)
                        break;
                    underline = true;
                    r.d += "<u>";
                    break;
                case '|':
                    break;
                case '@':
                    break;
                case 0x1A:
                    break;
                case 0x19:
                    break;
                default:
                    --it;
                    return;
                }
                ++it;
            }
        };

        auto clearAttr = [&r, &bold, &reverse, &italic, &underline, &keep](QByteArray::iterator &it) {
            while (true) {
                switch(*it) {
                case '*':
                    if (bold) {
                        bold = false;
                        r.d += "</b>";
                    }
                    break;
                case '!':
                    if (reverse) {
                        reverse = false;
                        // TODO
                    }
                    break;
                case '/':
                    if (italic) {
                        italic = false;
                        r.d += "</i>";
                    }
                    break;
                case '_':
                    if (underline) {
                        underline = false;
                        r.d += "</u>";
                    }
                    break;
                case '|':
                    break;
                case 0x1B:
                    break;
                default:
                    --it;
                    return;
                }
                ++it;
            }
        };
        auto loadStd = [&r, &foreground](QByteArray::iterator &it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
                ++it;
            int code = 0;
            if (*it == 0x19 || *it == 'F')
                it++;
            for (int i = 0; i < 2; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (foreground)
                r.d += "</font>";
            if (weechatColors.contains(code)) {
                r.d += "<font color=\""+weechatColors[code]+"\">";
                foreground = true;
            }
        };
        auto loadExt = [&r, &foreground](QByteArray::iterator &it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
                ++it;
            int code = 0;
            for (int i = 0; i < 5; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (foreground)
                r.d += "</font>";
            r.d += "<font color=\"green\">";
            foreground = true;
        };
        auto loadBgStd = [&r, &background](QByteArray::iterator &it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
                ++it;
            int code = 0;
            for (int i = 0; i < 2; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (background)
                r.d += "</span";
            r.d += "<span style=\"background-color: blue\">";
            background = true;
        };
        auto loadBgExt = [&r, &background](QByteArray::iterator &it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
                ++it;
            int code = 0;
            for (int i = 0; i < 5; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (background)
                r.d += "</span";
            r.d += "<span style=\"background-color: red\">";
            background = true;
        };
        auto getChar = [](QByteArray::iterator &it) -> QString {
            if ((unsigned char) *it < 0x80) {
                return QString(*it);
            }
            else {
                QByteArray buf;
                if ((*it & 0b11111000) == 0b11110000) {
                    buf += *it++;
                    buf += *it++;
                    buf += *it++;
                    buf += *it;
                }
                else if ((*it & 0b11110000) == 0b11100000) {
                    buf += *it++;
                    buf += *it++;
                    buf += *it;
                }
                else if ((*it & 0b11100000) == 0b11000000) {
                    buf += *it++;
                    buf += *it;
                }
                else {
                    return QString(*it);
                }
                return QString(buf);
            }
        };
        for (auto it = buf.begin(); it != buf.end(); ++it) {
            if (*it == 0x19) {
                ++it;
                if (*it == 'F') {
                    ++it;
                    if (*it == '@') {
                        ++it;
                        loadAttr(it);
                        loadExt(it);
                    }
                    else {
                        loadAttr(it);
                        loadStd(it);
                    }
                }
                else if (*it == 'B') {
                    ++it;
                    if (*it == '@')
                        loadBgExt(it);
                    else
                        loadBgStd(it);
                }
                else if (*it == '*') {
                    ++it;
                    if (*it == '@') {
                        ++it;
                        loadAttr(it);
                        loadExt(it);
                    }
                    else {
                        loadAttr(it);
                        loadExt(it);
                    }
                    if (*it == ',' || *it == '~')
                        ++it;
                    if (*it == '@') {
                        ++it;
                        loadAttr(it);
                        loadExt(it);
                    }
                    else {
                        loadAttr(it);
                        loadExt(it);
                    }
                }
                else if (*it == '@') {
                    ++it;
                    loadExt(it);
                }
                else if (*it == 0x1C) {
                    endColors();
                }
                else {
                    loadStd(it);
                }
            }
            else if (*it == 0x1C) {
                ++it;
                endColors();
                endAttrs();
            }
            else if (*it == 0x1A) {
                loadAttr(it);
            }
            else if (*it == 0x1B) {
                clearAttr(it);
            }
            else if (*it) {
                r.d += getChar(it);
            }
        }
        endColors();
        endAttrs();
    }
    return true;
}

bool W::parse(QDataStream &s, W::Buffer &r) {
    uint32_t len;
    r.d.clear();
    s >> len;
    if (len == 0)
        r.d = "";
    if (len > 0) {
        r.d = QByteArray(len, 0);
        s.readRawData(r.d.data(), len);
    }
    return true;
}

bool W::parse(QDataStream &s, W::Pointer &r) {
    quint8 length;;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    r.d = buf.toULongLong(nullptr, 16);
    return true;
}

bool W::parse(QDataStream &s, W::Time &r) {
    quint8 length;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    r.d = buf;
    return true;
}

bool W::parse(QDataStream &s, W::HashTable &r) {
    return true;
}

bool W::parse(QDataStream &s, W::HData &r) {
    //qCritical() << type;
    W::String hpath;
    W::String keys;
    W::Integer count;
    parse(s, hpath);
    parse(s, keys);
    parse(s, count);
    //qCritical() << hpath.d << keys.d << count.d;
    for (int i = 0; i < count.d; i++) {
        qApp->eventDispatcher()->processEvents(QEventLoop::AllEvents);
        QStringList pathElems = hpath.d.split("/");
        QStringList arguments = keys.d.split(",");
        pointer_t parentPtr = 0;
        pointer_t stuffPtr = 0;

        pointer_t bufferPtr = 0;
        pointer_t linePtr = 0;
        pointer_t nickPtr = 0;
        pointer_t hotListPtr = 0;
        for (int i = 0; i < pathElems.count(); i++) { // TODO
            W::Pointer ptr;
            parse(s, ptr);
            if (pathElems[i] == "buffer") {
                bufferPtr = ptr.d;
            }
            else if (pathElems[i] == "line_data") {
                linePtr = ptr.d;
            }
            else if (pathElems[i] == "nicklist_item"){
                nickPtr = ptr.d;
            }
            else if (pathElems[i] == "hotlist") {
                hotListPtr = ptr.d;
            }
            if (i == 2 && hpath.d != "buffer/lines/line/line_data") {
                qCritical() << "OMG got three stuff path";
                exit(1);
            }
        }
        if (hotListPtr != 0) {
            stuffPtr = hotListPtr;
        }
        else if (linePtr != 0) {
            stuffPtr = linePtr;
        }
        else if (bufferPtr != 0) {
            stuffPtr = bufferPtr;
        }
        if (nickPtr != 0) {
            stuffPtr = nickPtr;
            parentPtr = bufferPtr;
        }
        for (auto i : arguments) {
            qApp->eventDispatcher()->processEvents(QEventLoop::AllEvents);
            QStringList argument = i.split(":");
            QString name = argument[0];
            QString type = argument[1];
            if (pathElems.last() == "hotlist") {
                //qCritical() << pathElems << arguments;
            }
            if (type == "int") {
                W::Integer i;
                parse(s, i);
                //qCritical() << name << ":" << i.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(i.d));
            }
            else if (type == "lon") {
                W::LongInteger l;
                parse(s, l);
                //qCritical() << name << ":" << l.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(l.d));
            }
            else if (type == "str" || type == "buf") {
                W::String str;
                parse(s, str);
                //qCritical () << name << ":" << str.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(str.d));
            }
            else if (type == "arr") {
                char type[4] = { 0 };
                s.readRawData(type, 3);
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                //qCritical() << name << ":" << QString(type);
                if (strcmp(type, "int") == 0) {
                    W::ArrayInt a;
                    parse(s, a);
                    //qCritical() << name << ":" << a.d;
                    if (stuff && stuff->property(qPrintable(name)).isValid())
                        stuff->setProperty(qPrintable(name), QVariant::fromValue(a.d));
                }
                if (strcmp(type, "str") == 0) {
                    W::ArrayStr a;
                    parse(s, a);
                    //qCritical() << name << ":" << a.d;
                    if (stuff && stuff->property(qPrintable(name)).isValid())
                        stuff->setProperty(qPrintable(name), QVariant::fromValue(a.d));
                }
            }
            else if (type == "tim") {
                W::Time t;
                parse(s, t);
                //qCritical() << name << ":" << t.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(QDateTime::fromMSecsSinceEpoch(t.d.toLongLong() * 1000)));
            }
            else if (type == "ptr") {
                W::Pointer p;
                parse(s, p);
                //qCritical() << name << ":" << p.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                QObject *otherStuff = Lith::instance()->getObject(p.d, "");
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(otherStuff));
            }
            else if (type == "chr") {
                W::Char c;
                parse(s, c);
                //qCritical() << name << ":" << c.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(c.d));
            }
            else {
                //qCritical() << "Type was" << type;
            }
        }
    }
    return true;
}

bool W::parse(QDataStream &s, W::ArrayInt &r) {
    uint32_t len;
    s >> len;
    for (uint32_t i = 0; i < len; i++) {
        W::Integer num;
        parse(s, num);
        r.d.append(num.d);
    }
    return true;
}

bool W::parse(QDataStream &s, W::ArrayStr &r) {
    uint32_t len;
    s >> len;
    for (uint32_t i = 0; i < len; i++) {
        W::String str;
        parse(s, str);
        r.d.append(str.d);
    }
    return true;
}

Buffer::Buffer(QObject *parent, pointer_t pointer)
    : QObject(parent)
    , m_lines(QmlObjectList::create<BufferLine>(this))
    , m_ptr(pointer)
{

}

void Buffer::appendLine(BufferLine *line) {
    if (!line->dateGet().isValid()) {
        m_lines->prepend(line);
    }
    else {
        for (int i = 0; i < m_lines->count(); i++) {
            auto curr = m_lines->get<BufferLine>(i);
            if (curr && curr->dateGet() < line->dateGet()) {
                m_lines->insert(i, line);
                return;
            }
        }
        m_lines->append(line);
    }
}

bool Buffer::isAfterInitialFetch() {
    return m_afterInitialFetch;
}

QmlObjectList *Buffer::lines() {
    return m_lines;
}

QList<QObject *> Buffer::nicks() {
    return m_nicks;
}

Nick *Buffer::getNick(pointer_t ptr) {
    for (auto i : m_nicks) {
        auto n = qobject_cast<Nick*>(i);
        if (n->ptrGet() == ptr)
            return n;
    }

    auto nick = new Nick(this);
    m_nicks.append(nick);
    nick->ptrSet(ptr);
    emit nicksChanged();
    return nick;
}

QStringList Buffer::getVisibleNicks() {
    QStringList result;
    for (auto i : m_nicks) {
        auto nick = qobject_cast<Nick*>(i);
        if (nick->visibleGet() && nick->levelGet() == 0) {
            result.append(nick->nameGet());
        }
    }
    return result;
}

void Buffer::input(const QString &data) {
    Lith::instance()->weechat()->input(m_ptr, data);
}

void Buffer::fetchMoreLines() {
    m_afterInitialFetch = true;
    if (m_lines->count() >= m_lastRequestedCount) {
        Lith::instance()->weechat()->fetchLines(m_ptr, m_lines->count() + 25);
        m_lastRequestedCount = m_lines->count() + 25;
    }
}

BufferLine::BufferLine(QObject *parent)
    : QObject(parent)
{
    connect(this, &BufferLine::messageChanged, this, &BufferLine::onMessageChanged);
}

bool BufferLine::isPrivMsg() {
    return m_tags_array.contains("irc_privmsg");
}

QObject *BufferLine::bufferGet() {
    return parent();
}

void BufferLine::bufferSet(QObject *o) {
    Buffer *buffer = qobject_cast<Buffer*>(o);
    if (!buffer) {
        deleteLater();
        return;
    }
    if (qobject_cast<Buffer*>(parent()) != nullptr)
        return;

    if (parent() != o) {
        setParent(o);
        emit bufferChanged();
    }

    buffer->appendLine(this);
}

QList<QObject *> BufferLine::segments() {
    return m_segments;
}

void BufferLine::onMessageChanged() {
    QRegExp re("(?:(?:https?|ftp|file):\\/\\/|www\\.|ftp\\.)(?:\\([-A-Z0-9+&@#\\/%=~_|$?!:,.]*\\)|[-A-Z0-9+&@#\\/%=~_|$?!:,.])*(?:\\([-A-Z0-9+&@#\\/%=~_|$?!:,.]*\\)|[A-Z0-9+&@#\\/%=~_|$])", Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
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
    connect(this, &HotListItem::bufferChanged, [this](){
        onCountChanged();
    });
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
    QUrl url(plainTextGet());
    if (plainTextGet().startsWith("http") && url.isValid()) {
        QString extension = url.fileName().split(".").last().toLower();
        QString host = url.host();
        QString file = url.fileName();
        const auto maxUnshortenedLinkLength = 50;
        if (plainTextGet().size() > maxUnshortenedLinkLength && !file.isEmpty() && !host.isEmpty() && !extension.isEmpty())
            // \u2026 is the ellipsis character
            m_summary = url.scheme() + "://" + host + "/\u2026/" + file;
        else
            m_summary = plainTextGet();
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
