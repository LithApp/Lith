#include "weechat.h"

#include <QDataStream>
#include <QApplication>
#include <QImage>

Weechat *Weechat::_self = nullptr;
Weechat *Weechat::instance() {
    if (!_self)
        _self = new Weechat();
    return _self;
}

int Weechat::fetchFrom() {
    return m_fetchBuffer.count();
}

int Weechat::fetchTo() {
    return m_fetchBuffer.count() + m_bytesRemaining;
}

Weechat::Weechat(QObject *parent)
    : QObject(parent)
    , m_settings("Lith")
{
    statusSet(UNCONFIGURED);
    connect(this, &Weechat::settingsChanged, this, &Weechat::onSettingsChanged);

    m_host = m_settings.value("host", QString()).toString();
    m_port = m_settings.value("port", 9001).toInt();
    m_useEncryption = m_settings.value("encrypted", true).toBool();
    m_passphrase = m_settings.value("passphrase", QString()).toString();

    if (!m_host.isEmpty() && !m_passphrase.isEmpty()) {
        QTimer::singleShot(0, this, &Weechat::start);
    }

    m_reconnectTimer.setInterval(1000);
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &Weechat::start);
}

QString Weechat::host() const {
    return m_host;
}

int Weechat::port() const {
    return m_port;
}

bool Weechat::encrypted() const {
    return m_useEncryption;
}

bool Weechat::hasPassphrase() const {
    return !m_passphrase.isEmpty();
}

void Weechat::start() {
    if (m_connection) {
        m_connection->deleteLater();
        m_connection = nullptr;
    }

    statusSet(CONNECTING);
    m_connection = new QSslSocket(this);
    m_connection->ignoreSslErrors({QSslError::UnableToGetLocalIssuerCertificate});

    connect(m_connection, static_cast<void(QSslSocket::*)(QSslSocket::SocketError)>(&QAbstractSocket::error), this, &Weechat::onError);
    connect(m_connection, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), this, &Weechat::onSslErrors);
    connect(m_connection, &QSslSocket::readyRead, this, &Weechat::onReadyRead);
    connect(m_connection, &QSslSocket::connected, this, &Weechat::onConnected);
    connect(m_connection, &QSslSocket::disconnected, this, &Weechat::onDisconnected);

    if (m_useEncryption)
        m_connection->connectToHostEncrypted(m_host, m_port);
    else
        m_connection->connectToHost(m_host, m_port);
}

void Weechat::setHost(const QString &value) {
    if (m_host != value) {
        m_host = value;
        m_settings.setValue("host", m_host);
        emit settingsChanged();
    }
}

void Weechat::setPort(int value) {
    if (m_port != value) {
        m_port = value;
        m_settings.setValue("port", m_port);
        emit settingsChanged();
    }
}

void Weechat::setEncrypted(bool value) {
    if (m_useEncryption != value) {
        m_useEncryption = value;
        m_settings.setValue("encrypted", m_useEncryption);
        emit settingsChanged();
    }
}

void Weechat::setPassphrase(const QString &value) {
    if (m_passphrase != value) {
        m_passphrase = value;
        m_settings.setValue("passphrase", m_passphrase);
        emit settingsChanged();
        emit hasPassphraseChanged();
    }
}

void Weechat::onSettingsChanged() {
    if (!m_host.isEmpty() && !m_passphrase.isEmpty()) {
        if (m_connection) {
            m_connection->deleteLater();
            StuffManager::instance()->reset();
        }
        QTimer::singleShot(0, this, &Weechat::start);
    }
}

void Weechat::requestHotlist() {
    m_connection->write("hdata hotlist:gui_hotlist(*)\n");
}

void Weechat::onReadyRead() {
    static bool compressed = false;

    QByteArray tmp;

    if (m_bytesRemaining > 0) {
        tmp = m_connection->read(m_bytesRemaining);
        m_bytesRemaining -= tmp.length();
        m_fetchBuffer.append(tmp);
        emit fetchFromChanged();
    }
    else {
        tmp = m_connection->readAll();
        QDataStream s(&tmp, QIODevice::ReadOnly);
        s >> m_bytesRemaining >> compressed;
        m_bytesRemaining -= tmp.length();
        m_fetchBuffer = tmp.mid(5);
        emit fetchFromChanged();
        emit fetchToChanged();
    }

    if (m_bytesRemaining == 0) {
        if (compressed) {
            // TODO decompress
        }
        onMessageReceived(m_fetchBuffer);
        m_fetchBuffer.clear();
        emit fetchToChanged();
        emit fetchFromChanged();
    }
}

void Weechat::onConnected() {
    qDebug() << "Connected!";
    statusSet(CONNECTED);
    m_connection->write(("init password=" + m_passphrase + ",compression=off\n").toUtf8());
    m_connection->write("hdata buffer:gui_buffers(*) number,name,hidden,title\n");
    m_connection->write("hdata buffer:gui_buffers(*)/lines/last_line(-3)/data\n");
    m_connection->write("hdata hotlist:gui_hotlist(*)\n");
    m_connection->write("sync\n");
    m_connection->write("nicklist\n");


    connect(&m_hotlistTimer, SIGNAL(timeout()), this, SLOT(requestHotlist()));
    m_hotlistTimer.setInterval(1000);
    m_hotlistTimer.setSingleShot(false);
    m_hotlistTimer.start();
}

void Weechat::onDisconnected() {
    statusSet(DISCONNECTED);

    if (m_reconnectTimer.interval() < 16 * 60 * 1000)
        m_reconnectTimer.setInterval(m_reconnectTimer.interval() * 2);
    m_reconnectTimer.start();
}

void Weechat::onError(QAbstractSocket::SocketError e) {
    qWarning() << "Error!" << e;
    statusSet(ERROR);
    errorStringSet(QString("Connection failed: %1").arg(m_connection->errorString()));

    if (m_reconnectTimer.interval() < 16 * 60 * 1000)
        m_reconnectTimer.setInterval(m_reconnectTimer.interval() * 2);
    m_reconnectTimer.start();
}

void Weechat::onSslErrors(const QList<QSslError> errors) {
    m_connection->ignoreSslErrors(errors);
    for (auto i : errors) {
        qWarning() << "SSL Error!" << i.errorString();
    }
}

void Weechat::onMessageReceived(QByteArray &data) {
    //qDebug() << "Message!" << data;
    QDataStream s(&data, QIODevice::ReadOnly);

    W::String id;
    s >> id;

    //qDebug() << "=== TYPE" << id.d;

        W::HData hda;
        s >> hda;
}

void Weechat::input(pointer_t ptr, const QString &data) {
    QString line = QString("input 0x%1 %2\n").arg(ptr, 0, 16).arg(data);
    //qCritical() << "WRITING:" << line;
    m_connection->write(line.toUtf8());
}

void Weechat::fetchLines(pointer_t ptr, int count) {
    QString line = QString("hdata buffer:0x%1/lines/last_line(-%2)/data\n").arg(ptr, 0, 16).arg(count);
    //qCritical() << "WRITING:" << line;
    m_connection->write(line.toUtf8());
}

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
    //qDebug() << type;
    W::String hpath;
    W::String keys;
    W::Integer count;
    s >> hpath >> keys >> count;
    //qDebug() << hpath.d << keys.d << count.d;
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
                //qDebug() << name << ":" << i.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(i.d));
            }
            else if (type == "lon") {
                W::LongInteger l;
                s >> l;
                //qDebug() << name << ":" << l.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(l.d));
            }
            else if (type == "str") {
                W::String str;
                s >> str;
                //qDebug () << name << ":" << str.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(str.d));
            }
            else if (type == "arr") {
                char type[4] = { 0 };
                s.readRawData(type, 3);
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                //qDebug() << name << ":" << QString(type);
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
                    //qDebug() << name << ":" << a.d;
                    if (stuff && stuff->property(qPrintable(name)).isValid())
                        stuff->setProperty(qPrintable(name), QVariant::fromValue(a.d));
                }
            }
            else if (type == "tim") {
                W::Time t;
                s >> t;
                //qDebug() << name << ":" << t.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(QDateTime::fromMSecsSinceEpoch(t.d.toLongLong() * 1000)));
            }
            else if (type == "ptr") {
                W::Pointer p;
                s >> p;
                //qDebug() << name << ":" << p.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                QObject *otherStuff = StuffManager::instance()->getStuff(p.d, "");
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(otherStuff));
            }
            else if (type == "chr") {
                W::Char c;
                s >> c;
                //qDebug() << name << ":" << c.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(c.d));
            }
            else {
                //qDebug() << "Type was" << type;
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

StuffManager *StuffManager::_self = nullptr;
StuffManager *StuffManager::instance() {
    if (!_self)
        _self = new StuffManager();
    return _self;
}

QObject *StuffManager::getStuff(pointer_t ptr, const QString &type, pointer_t parent) {
    if (type.isEmpty()) {
        if (m_bufferMap.contains(ptr))
            return m_bufferMap[ptr];
        if (m_lineMap.contains(ptr))
            return m_lineMap[ptr];
        return nullptr;
    }
    else if (type == "buffer") {
        if (!m_bufferMap.contains(ptr)) {
            beginInsertRows(QModelIndex(), m_buffers.count(), m_buffers.count());
            Buffer *tmp = new Buffer(this, ptr);
            m_bufferMap[ptr] = tmp;
            m_buffers.append(tmp);
            endInsertRows();
            emit buffersChanged();
            if (m_buffers.count() == 1)
                emit selectedChanged();
        }
        return m_bufferMap[ptr];
    }
    else if (type == "line_data") {/*
        if (m_bufferMap.contains(parent)) {
            //Buffer *buffer = m_bufferMap[parent];
            //return buffer->getLine(ptr);
        }
        else if (parent == 0) { */
            if (!m_lineMap.contains(ptr))
                m_lineMap[ptr] = new BufferLine(this);
            return m_lineMap[ptr];
            /*
        }
        else {
            qCritical() << "Got line information before the buffer was allocated";
        }
        */
    }
    else if (type == "nicklist_item") {
        if (m_bufferMap.contains(parent)) {
            return m_bufferMap[parent]->getNick(ptr);
        }
    }
    else if (type == "hotlist") {
        //qCritical() << ptr;
        if (!m_hotList.contains(ptr))
            m_hotList.insert(ptr, new HotListItem(this));
        return m_hotList[ptr];
    }
    else {
        //qCritical() << "Unknown type of new stuff requested:" << type;
    }
    return nullptr;
}

/*
QQmlListProperty<QObject> StuffManager::buffers() {
    static QList<QObject *> ret;
    ret.clear();
    for (auto i : m_buffers) {
        ret.append(i);
    }
    return QQmlListProperty<QObject>(this, ret);
}
*/

int StuffManager::bufferCount() {
    return m_buffers.count();
}

Buffer *StuffManager::selectedBuffer() {
    if (m_selectedIndex >=0 && m_buffers.count() > 4)
        return m_buffers[m_selectedIndex];
    return nullptr;
}

int StuffManager::selectedIndex() {
    return m_selectedIndex;
}

void StuffManager::setSelectedIndex(int o) {
    if (m_selectedIndex != o) {
        m_selectedIndex = o;
        emit selectedChanged();
        if (selectedBuffer() && !selectedBuffer()->isAfterInitialFetch())
            selectedBuffer()->fetchMoreLines();
    }
}

QHash<int, QByteArray> StuffManager::roleNames() const {
    return { { Qt::UserRole, "buffer" } };
}

int StuffManager::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_buffers.count();
}

QVariant StuffManager::data(const QModelIndex &index, int role) const {
    Q_UNUSED(role);
    return QVariant::fromValue(m_buffers[index.row()]);
}

void StuffManager::reset() {
    for (auto i : m_buffers) {
        i->deleteLater();
    }
    m_buffers.clear();
    m_bufferMap.clear();
}

StuffManager::StuffManager() : QAbstractListModel() {
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
    qDebug() << "Line added!";
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
        m_segments.append(new BufferLineSegment(this, messageGet().mid(lastIdx, length).trimmed()));
        m_segments.append(new BufferLineSegment(this, re.cap().trimmed(), BufferLineSegment::LINK));

        lastIdx = idx + re.matchedLength();
    }
    if (lastIdx < messageGet().length())
        m_segments.append(new BufferLineSegment(this, messageGet().mid(lastIdx).trimmed()));

    emit segmentsChanged();
}

Nick::Nick(Buffer *parent)
    : QObject(parent)
{

}

ClipboardProxy::ClipboardProxy(QObject *parent)
    : QObject(parent)
    , m_clipboard(QApplication::clipboard())
{
}

bool ClipboardProxy::hasImage() {
    return !m_clipboard->image().isNull();
}

QString ClipboardProxy::text() {
    return m_clipboard->text();
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
        if (!url.fileName().isEmpty() && !url.host().isEmpty() && !extension.isEmpty())
            m_summary = url.host() + "-" + url.fileName();
        else
            m_summary = plainTextGet();
        if (QStringList{"png", "jpg", "gif"}.indexOf(extension) != -1)
            m_type = IMAGE;
        else if (QStringList{"avi", "mov", "mp4", "webm"}.indexOf(extension) != -1)
            m_type = VIDEO;
        else
            m_type = LINK;
        // youtube: "https://www.youtube.com/embed/IDidIDidIDi"
    }
}
