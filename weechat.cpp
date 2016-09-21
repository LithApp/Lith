#include "weechat.h"

#include <QDataStream>

Weechat *Weechat::_self = nullptr;
Weechat *Weechat::instance() {
    if (!_self)
        _self = new Weechat();
    return _self;
}

Weechat::Weechat(QObject *parent)
    : QObject(parent)
    , m_settings("Lith")
{
    connect(this, &Weechat::settingsChanged, this, &Weechat::onSettingsChanged);

    m_host = m_settings.value("host", QString()).toString();
    m_port = m_settings.value("port", 9001).toInt();
    m_useEncryption = m_settings.value("encrypted", true).toBool();
    m_passphrase = m_settings.value("passphrase", QString()).toString();

    if (!m_host.isEmpty() && !m_passphrase.isEmpty()) {
        QTimer::singleShot(0, this, &Weechat::start);
    }
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

void Weechat::start() {
    if (m_connection)
        m_connection->deleteLater();
    m_connection = new QSslSocket(this);

    connect(m_connection, static_cast<void(QSslSocket::*)(QSslSocket::SocketError)>(&QAbstractSocket::error), this, &Weechat::onError);
    connect(m_connection, &QSslSocket::readyRead, this, &Weechat::onReadyRead);
    connect(m_connection, &QSslSocket::connected, this, &Weechat::onConnected);

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

void Weechat::onReadyRead() {
    static QByteArray buffer;
    static uint32_t remaining = 0;
    static bool compressed = false;
    qCritical() << "Reading" << buffer.count() << "of" << buffer.count() + remaining;

    QByteArray tmp;

    if (remaining > 0) {
        tmp = m_connection->read(remaining);
        remaining -= tmp.length();
        buffer.append(tmp);
    }
    else {
        tmp = m_connection->readAll();
        QDataStream s(&tmp, QIODevice::ReadOnly);
        s >> remaining >> compressed;
        remaining -= tmp.length();
        buffer = tmp.mid(5);
    }

    if (remaining == 0) {
        if (compressed) {
            // TODO decompress
        }
        onMessageReceived(buffer);
        buffer.clear();
    }
}

void Weechat::onConnected() {
    qDebug() << "Connected!";
    m_connection->write(("init password=" + m_passphrase + ",compression=off\n").toUtf8());
    m_connection->write("hdata buffer:gui_buffers(*) number,name,hidden,title\n");
    m_connection->write("hdata buffer:gui_buffers(*)/lines/last_line(-3)/data\n");
    //m_connection->write("hdata hotlist:gui_hotlist(*)\n");
    m_connection->write("sync\n");
}

void Weechat::onError(QAbstractSocket::SocketError e) {
    qWarning() << "Error!" << e;
}

void Weechat::onMessageReceived(QByteArray &data) {
    qDebug() << "Message!" << data;
    QDataStream s(&data, QIODevice::ReadOnly);

    W::String id;
    s >> id;

    qDebug() << "=== TYPE" << id.d;

        W::HData hda;
        s >> hda;
}

void Weechat::input(pointer_t ptr, const QString &data) {
    QString line = QString("input 0x%1 %2\n").arg(ptr, 0, 16).arg(data);
    qCritical() << "WRITING:" << line;
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
    if (len == 0)
        r.d = "";
    if (len > 0) {
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
    qDebug() << type;
    W::String hpath;
    W::String keys;
    W::Integer count;
    s >> hpath >> keys >> count;
    qDebug() << hpath.d << keys.d << count.d;
    for (int i = 0; i < count.d; i++) {
        QStringList pathElems = hpath.d.split("/");
        QStringList arguments = keys.d.split(",");
        pointer_t parentPtr = 0;
        pointer_t stuffPtr = 0;

        pointer_t bufferPtr = 0;
        pointer_t linePtr = 0;
        for (int i = 0; i < pathElems.count(); i++) { // TODO
            W::Pointer ptr;
            s >> ptr;
            if (pathElems[i] == "buffer") {
                bufferPtr = ptr.d;
            }
            if (pathElems[i] == "line_data") {
                linePtr = ptr.d;
            }
            if (i == 2 && hpath.d != "buffer/lines/line/line_data") {
                qCritical() << "OMG got three stuff path";
                exit(1);
            }
        }
        if (linePtr != 0) {
            stuffPtr = linePtr;
        }
        else if (bufferPtr != 0) {
            stuffPtr = bufferPtr;
        }
        for (auto i : arguments) {
            QStringList argument = i.split(":");
            QString name = argument[0];
            QString type = argument[1];
            if (type == "int") {
                W::Integer i;
                s >> i;
                qDebug() << name << ":" << i.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff)
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(i.d));
            }
            else if (type == "str") {
                W::String str;
                s >> str;
                qDebug () << name << ":" << str.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff)
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(str.d));
            }
            else if (type == "arr") {
                char type[4] = { 0 };
                s.readRawData(type, 3);
                if (strcmp(type, "int") == 0) {
                    W::ArrayInt a;
                    s >> a;
                    qDebug() << name << ":" << a.d;
                }
                if (strcmp(type, "str") == 0) {
                    W::ArrayStr a;
                    s >> a;
                    qDebug() << name << ":" << a.d;
                }
            }
            else if (type == "tim") {
                W::Time t;
                s >> t;
                qDebug() << name << ":" << t.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff)
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(QDateTime::fromMSecsSinceEpoch(t.d.toLongLong() * 1000)));
            }
            else if (type == "ptr") {
                W::Pointer p;
                s >> p;
                qDebug() << name << ":" << p.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                QObject *otherStuff = StuffManager::instance()->getStuff(p.d, "");
                if (stuff)
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(otherStuff));
            }
            else if (type == "chr") {
                W::Char c;
                s >> c;
                qDebug() << name << ":" << c.d;
                QObject *stuff = StuffManager::instance()->getStuff(stuffPtr, pathElems.last(), parentPtr);
                if (stuff)
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(c.d));
            }
            else {
                qDebug() << "Type was" << type;
            }
        }
    }
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::ArrayInt &r) {
    uint32_t len;
    s >> len;
    for (int i = 0; i < len; i++) {
        W::Integer num;
        s >> num;
        r.d.append(num.d);
    }
    return s;
}

QDataStream &W::operator>>(QDataStream &s, W::ArrayStr &r) {
    uint32_t len;
    s >> len;
    for (int i = 0; i < len; i++) {
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
    if (m_buffers.count() > 4)
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
    }
}

QHash<int, QByteArray> StuffManager::roleNames() const {
    return { { Qt::UserRole, "buffer" } };
}

int StuffManager::rowCount(const QModelIndex &parent) const {
    return m_buffers.count();
}

QVariant StuffManager::data(const QModelIndex &index, int role) const {
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

LineModel::LineModel(Buffer *parent) {

}

QHash<int, QByteArray> LineModel::roleNames() const {
    return { { Qt::UserRole, "line" } };
}

int LineModel::rowCount(const QModelIndex &parent) const {
    return m_lines.count();
}

QVariant LineModel::data(const QModelIndex &index, int role) const {
    return QVariant::fromValue(m_lines[index.row()]);
}

void LineModel::appendLine(BufferLine *line) {
    if (!line->dateGet().isValid()) {
        beginInsertRows(QModelIndex(), m_lines.count(), m_lines.count());
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
        beginInsertRows(QModelIndex(), pos, pos);
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

LineModel *Buffer::lines() {
    return m_lines;
}

void Buffer::input(const QString &data) {
    Weechat::instance()->input(m_ptr, data);
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

QObject *BufferLine::bufferGet() {
    return parent();
}

void BufferLine::bufferSet(QObject *o) {
    Buffer *buffer = qobject_cast<Buffer*>(o);

    if (parent() != o) {
        setParent(o);
        emit bufferChanged();
    }

    buffer->appendLine(this);
}
