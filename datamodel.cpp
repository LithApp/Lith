#include "datamodel.h"

#include "weechat.h"

#include <QDataStream>
#include <QUrl>

#include <QDebug>

QDataStream &W::operator>>(QDataStream &s, W::Char &r) {
    s.readRawData(&r.d, 1);
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::Integer &r) {
    s.setByteOrder(QDataStream::BigEndian);
    s >> r.d;
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::LongInteger &r) {
    Char c;
    s >> c;
    QByteArray buf((int) c.d + 1, 0);
    s.readRawData(buf.data(), c.d);
    r.d = buf.toLongLong();
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::String &r) {
    uint32_t len;
    r.d.clear();
    s >> len;
    if (len == uint32_t(-1))
        r.d = QString();
    else if (len == 0)
        r.d = "";
    else if (len > 0) {
        QByteArray buf(len + 1, 0);
        s.readRawData(buf.data(), len);
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
    }
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::Buffer &r) {
    uint32_t len;
    r.d.clear();
    s >> len;
    if (len == 0)
        r.d = "";
    if (len > 0) {
        r.d = QByteArray(len, 0);
        s.readRawData(r.d.data(), len);
    }
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::Pointer &r) {
    Char c;
    s >> c;
    QByteArray buf((int) c.d + 1, 0);
    s.readRawData(buf.data(), c.d);
    r.d = buf.toULongLong(nullptr, 16);
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::Time &r) {
    Char c;
    s >> c;
    QByteArray buf((int) c.d + 1, 0);
    s.readRawData(buf.data(), c.d);
    r.d = buf;
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::HashTable &r) {
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::HData &r) {
    char type[4] = { 0 };
    s.readRawData(type, 3);
    //qCritical() << type;
    W::String hpath;
    W::String keys;
    W::Integer count;
    s >> hpath >> keys >> count;
    //qCritical() << hpath.d << keys.d << count.d;
    for (int i = 0; i < count.d; i++) {
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
            s >> ptr;
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
            QStringList argument = i.split(":");
            QString name = argument[0];
            QString type = argument[1];
            if (pathElems.last() == "hotlist") {
                //qCritical() << pathElems << arguments;
            }
            if (type == "int") {
                W::Integer i;
                s >> i;
                //qCritical() << name << ":" << i.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(i.d));
            }
            else if (type == "lon") {
                W::LongInteger l;
                s >> l;
                //qCritical() << name << ":" << l.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(l.d));
            }
            else if (type == "str") {
                W::String str;
                s >> str;
                //qCritical () << name << ":" << str.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(str.d));
            }
            else if (type == "arr") {
                char type[4] = { 0 };
                s.readRawData(type, 3);
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                //qCritical() << name << ":" << QString(type);
                if (strcmp(type, "int") == 0) {
                    W::ArrayInt a;
                    s >> a;
                    //qCritical() << name << ":" << a.d;
                    if (stuff && stuff->property(qPrintable(name)).isValid())
                        stuff->setProperty(qPrintable(name), QVariant::fromValue(a.d));
                }
                if (strcmp(type, "str") == 0) {
                    W::ArrayStr a;
                    s >> a;
                    //qCritical() << name << ":" << a.d;
                    if (stuff && stuff->property(qPrintable(name)).isValid())
                        stuff->setProperty(qPrintable(name), QVariant::fromValue(a.d));
                }
            }
            else if (type == "tim") {
                W::Time t;
                s >> t;
                //qCritical() << name << ":" << t.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(QDateTime::fromMSecsSinceEpoch(t.d.toLongLong() * 1000)));
            }
            else if (type == "ptr") {
                W::Pointer p;
                s >> p;
                //qCritical() << name << ":" << p.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                QObject *otherStuff = StuffManager::instance()->getStuff(p.d, "");
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(otherStuff));
            }
            else if (type == "chr") {
                W::Char c;
                s >> c;
                //qCritical() << name << ":" << c.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(c.d));
            }
            else {
                //qCritical() << "Type was" << type;
            }
        }
    }
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::ArrayInt &r) {
    uint32_t len;
    s >> len;
    for (uint32_t i = 0; i < len; i++) {
        W::Integer num;
        s >> num;
        r.d.append(num.d);
    }
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::ArrayStr &r) {
    uint32_t len;
    s >> len;
    for (uint32_t i = 0; i < len; i++) {
        W::String str;
        s >> str;
        r.d.append(str.d);
    }
    return s;
}

LineModel::LineModel(Buffer *parent)
    : QAbstractListModel(parent)
{

}

QHash<int, QByteArray> LineModel::roleNames() const {
    return { { Qt::UserRole, "line" }, { Qt::UserRole + 1, "sender" } };
}

int LineModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_lines.count();
}

QVariant LineModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::UserRole)
        return QVariant::fromValue(m_lines[m_lines.count() - 1 - index.row()]);
    else
        return QVariant::fromValue(m_lines[m_lines.count() - 1 - index.row()]->prefixGet());
}

void LineModel::appendLine(BufferLine *line) {
    if (!line->dateGet().isValid()) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_lines.append(line);
        endInsertRows();
    }
    else {
        int pos = 0;
        QList<BufferLine*>::iterator iterator = m_lines.begin();
        for (auto i = m_lines.begin(); i != m_lines.end(); i++) {
            if ((*i)->dateGet() > line->dateGet()) {
                iterator = i;
                break;
            }
            pos++;
        }
        beginInsertRows(QModelIndex(), m_lines.count() - pos, m_lines.count() - pos);
        m_lines.insert(iterator, line);
        endInsertRows();
    }
}


Buffer::Buffer(QObject *parent, pointer_t pointer)
    : QObject(parent)
    , m_lines(new LineModel(this))
    , m_ptr(pointer)
{

}

/*
BufferLine *Buffer::getLine(pointer_t ptr) {
    qCritical() << "Line added!";
    if (!m_lineMap.contains(ptr)) {
        BufferLine *tmp = new BufferLine(this);
        m_lines->appendLine(tmp);
    }
    return m_lineMap[ptr];
}
*/

void Buffer::appendLine(BufferLine *line) {
    m_lines->appendLine(line);
}

bool Buffer::isAfterInitialFetch() {
    return m_afterInitialFetch;
}

LineModel *Buffer::lines() {
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

void Buffer::input(const QString &data) {
    Weechat::instance()->input(m_ptr, data);
}

void Buffer::fetchMoreLines() {
    m_afterInitialFetch = true;
    Weechat::instance()->fetchLines(m_ptr, m_lines->rowCount() + 25);
}

/*
Buffer *BufferLine::buffer() {
    return qobject_cast<Buffer*>(parent());
}

void BufferLine::setBuffer(Buffer *o) {
    if (parent() != o) {
        setParent(o);
        emit bufferChanged();
    }
    o->appendLine(this);
}
*/

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

void HotListItem::bufferSet(Buffer *o) {
    if (m_buffer != o) {
        m_buffer = o;
        emit bufferChanged();
    }
}

void HotListItem::onCountChanged() {
    if (bufferGet()) {
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
        if (!file.isEmpty() && !host.isEmpty() && !extension.isEmpty())
            m_summary = url.host() + "-" + url.fileName();
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
