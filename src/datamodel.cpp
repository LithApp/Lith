#include "datamodel.h"

#include "weechat.h"
#include "lith.h"

#include <QUrl>
#include <QApplication>
#include <QTextDocumentFragment>


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

QString BufferLine::colorlessNicknameGet() {
    QString nickStripped = QTextDocumentFragment::fromHtml(m_prefix).toPlainText();
    return nickStripped;
}

QString BufferLine::colorlessTextGet() {
    QString messageStripped = QTextDocumentFragment::fromHtml(m_message).toPlainText();
    return messageStripped;
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
    connect(Lith::instance()->settingsGet(), &Settings::shortenLongUrlsThresholdChanged, this, &BufferLineSegment::summaryChanged);
    QUrl url(plainTextGet());
    if (plainTextGet().startsWith("http") && url.isValid()) {
        QString extension = url.fileName().split(".").last().toLower();
        QString host = url.host();
        QString file = url.fileName();

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
    if (plainTextGet().size() < threshold
            || !plainTextGet().startsWith("http")) {
        return plainTextGet();
    }
    QUrl url(plainTextGet());
    if (!url.isValid()) {
        return plainTextGet();
    }

    QString extension = url.fileName().split(".").last().toLower();
    QString scheme = url.scheme();
    QString host = url.host();
    QString file = url.fileName();
    QString query = url.query();
    const auto ellipsis = "\u2026";
    // We'll show always show the host (and the scheme, if it's present)
    const auto hostPrefix = (!scheme.isEmpty() ? scheme + "://" : "") + host + "/" + ellipsis;
    qDebug() << scheme << host << file << query;

    // This is a classic "URL to a file", for example a link to an image on an image-hosting website, we'll make sure
    // the shortened path includes the filename.
    if (!file.isEmpty() && !host.isEmpty() && !extension.isEmpty() && !query.isEmpty()) {
        return hostPrefix + file;
    }

    // Otherwise it's a weird link with queries and stuff. We'll just shorten the whole thing and leave just enough
    // characters so that we are within the threshold (but at least 10).
    const auto suffixCharacterCount = std::max(threshold - hostPrefix.length(), 10);
    return hostPrefix + plainTextGet().right(suffixCharacterCount);
}
