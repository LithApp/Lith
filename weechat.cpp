#include "weechat.h"

#include <QDataStream>
#include <QApplication>
#include <QImage>

Lith *Lith::_self = nullptr;
Lith *Lith::instance() {
    if (!_self)
        _self = new Lith();
    return _self;
}

ProxyBufferList *Lith::buffers() {
    return m_proxyBufferList;
}

Lith::Lith(QObject *parent)
    : QObject(parent)
    , m_proxyBufferList(new ProxyBufferList(this))
    , m_settings("Lith")
{
    statusSet(UNCONFIGURED);
    connect(this, &Lith::settingsChanged, this, &Lith::onSettingsChanged);

    m_host = m_settings.value("host", QString()).toString();
    m_port = m_settings.value("port", 9001).toInt();
    m_useEncryption = m_settings.value("encrypted", true).toBool();
    m_passphrase = m_settings.value("passphrase", QString()).toString();

    if (!m_host.isEmpty() && !m_passphrase.isEmpty()) {
        QTimer::singleShot(0, this, &Lith::start);
    }

    m_reconnectTimer.setInterval(1000);
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &Lith::restart);

    m_timeoutTimer.setInterval(5000);
    m_timeoutTimer.setSingleShot(true);
    connect(&m_timeoutTimer, &QTimer::timeout, this, &Lith::onTimeout);

    connect(&m_hotlistTimer, SIGNAL(timeout()), this, SLOT(requestHotlist()));
    m_hotlistTimer.setInterval(1000);
    m_hotlistTimer.setSingleShot(false);
}

QString Lith::host() const {
    return m_host;
}

int Lith::port() const {
    return m_port;
}

bool Lith::encrypted() const {
    return m_useEncryption;
}

bool Lith::hasPassphrase() const {
    return !m_passphrase.isEmpty();
}

void Lith::start() {
    if (m_connection) {
        m_connection->deleteLater();
        m_connection = nullptr;
    }
    qCritical() << "Connecting";

    statusSet(CONNECTING);
    m_connection = new QSslSocket(this);
    m_connection->ignoreSslErrors({QSslError::UnableToGetLocalIssuerCertificate});
    m_connection->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(m_connection, static_cast<void(QSslSocket::*)(QSslSocket::SocketError)>(&QAbstractSocket::errorOccurred), this, &Lith::onError);
    connect(m_connection, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), this, &Lith::onSslErrors);
    connect(m_connection, &QSslSocket::readyRead, this, &Lith::onReadyRead);
    connect(m_connection, &QSslSocket::connected, this, &Lith::onConnected);
    connect(m_connection, &QSslSocket::disconnected, this, &Lith::onDisconnected);

    if (m_useEncryption)
        m_connection->connectToHostEncrypted(m_host, m_port);
    else
        m_connection->connectToHost(m_host, m_port);
}

void Lith::restart() {
    qCritical() << "Reconnecting";
    if (m_useEncryption)
        m_connection->connectToHostEncrypted(m_host, m_port);
    else
        m_connection->connectToHost(m_host, m_port);
    m_reconnectTimer.stop();
}

void Lith::setHost(const QString &value) {
    if (m_host != value) {
        m_host = value;
        m_settings.setValue("host", m_host);
        emit settingsChanged();
    }
}

void Lith::setPort(int value) {
    if (m_port != value) {
        m_port = value;
        m_settings.setValue("port", m_port);
        emit settingsChanged();
    }
}

void Lith::setEncrypted(bool value) {
    if (m_useEncryption != value) {
        m_useEncryption = value;
        m_settings.setValue("encrypted", m_useEncryption);
        emit settingsChanged();
    }
}

void Lith::setPassphrase(const QString &value) {
    if (m_passphrase != value && !value.isEmpty()) {
        m_passphrase = value;
        m_settings.setValue("passphrase", m_passphrase);
        emit settingsChanged();
        emit hasPassphraseChanged();
    }
}

void Lith::onSettingsChanged() {
    if (!m_host.isEmpty() && !m_passphrase.isEmpty()) {
        if (m_connection) {
            m_connection->deleteLater();
            m_connection = nullptr;
            StuffManager::instance()->reset();
        }
        QTimer::singleShot(0, this, &Lith::start);
    }
}

void Lith::requestHotlist() {
    if (m_connection) {
        m_connection->write("hdata hotlist:gui_hotlist(*)\n");
    }
}

void Lith::onReadyRead() {
    m_timeoutTimer.stop();

    if (!m_connection) {
        // this shouldn't really happen, yet it seems it probably does
        return;
    }

    // In the protocol parser, there's the call to processEvents that could lead to
    // this slot being called while a message is already being processed
    // This is guard that prevents processing of more messages at the same moment
    static bool guard = false;
    if (guard)
        return;

    static bool compressed = false;


    // not waiting for the rest of any message, get a new header
    if (m_bytesRemaining == 0) {
        QByteArray header = m_connection->read(5);
        QDataStream s(&header, QIODevice::ReadOnly);
        s >> m_bytesRemaining >> compressed;
        m_bytesRemaining -= 5;
        m_fetchBuffer.clear();
    }

    // continue in whatever message came before (be it from a previous readyRead or this one
    if (m_bytesRemaining > 0) {
        QByteArray cache = m_connection->read(m_bytesRemaining);
        m_bytesRemaining -= cache.count();
        m_fetchBuffer.append(cache);
    }

    // one message has been received in full, process it
    if (m_bytesRemaining == 0) {
        if (compressed) {
            // TODO
        }
        guard = true;
        onMessageReceived(m_fetchBuffer);
        guard = false;
        m_fetchBuffer.clear();
    }

    // if there's still more data left, do one more round
    if (m_connection->bytesAvailable()) {
        onReadyRead();
    }
}

void Lith::onConnected() {
    qCritical() << "Connected!";
    statusSet(CONNECTED);
    m_connection->write(("init password=" + m_passphrase + ",compression=off\n").toUtf8());
    m_connection->write("hdata buffer:gui_buffers(*) number,name,hidden,title\n");
    m_connection->write("hdata buffer:gui_buffers(*)/lines/last_line(-1)/data\n");
    m_connection->write("hdata hotlist:gui_hotlist(*)\n");
    m_connection->write("sync\n");
    m_connection->write("nicklist\n");

    m_hotlistTimer.start();
}

void Lith::onDisconnected() {
    statusSet(DISCONNECTED);

    StuffManager::instance()->reset();
    m_hotlistTimer.stop();
    if (m_reconnectTimer.interval() < 5000)
        m_reconnectTimer.setInterval(m_reconnectTimer.interval() * 2);
    m_reconnectTimer.start();
}

void Lith::onError(QAbstractSocket::SocketError e) {
    qWarning() << "Error!" << e;
    statusSet(ERROR);
    errorStringSet(QString("Connection failed: %1").arg(m_connection->errorString()));

    if (m_reconnectTimer.interval() < 5000)
        m_reconnectTimer.setInterval(m_reconnectTimer.interval() * 2);
    m_reconnectTimer.start();
}

void Lith::onSslErrors(const QList<QSslError> &errors) {
    m_connection->ignoreSslErrors(errors);
    for (auto i : errors) {
        qWarning() << "SSL Error!" << i.errorString();
    }
}

void Lith::onMessageReceived(QByteArray &data) {
    //qCritical() << "Message!" << data;
    QDataStream s(&data, QIODevice::ReadOnly);

    // For whatever fucking reason, there sometimes seems to be an extra byte at the start of the message
    // this of course completely fucks parsing of absolutely everything
    // reading the extra byte beforehand may slithly help with this
    char wtf;
    if ((data[0] || data[1] || data[2]) && *(uint8_t*)data.data() != 0xff) {
        s.readRawData(&wtf, 1);
    }

    W::String id;
    s >> id;

    //qCritical() << "=== ID" << id.d;


    char type[4] = { 0 };
    s.readRawData(type, 3);

    if (QString(type) == "hda") {
        W::HData hda;
        s >> hda;
    }
    else {
        qCritical() << "onMessageReceived is not handling type: " << type;
    }
}

void Lith::input(pointer_t ptr, const QString &data) {
    QString line = QString("input 0x%1 %2\n").arg(ptr, 0, 16).arg(data);
    //qCritical() << "WRITING:" << line;
    m_connection->write(line.toUtf8());
}

void Lith::fetchLines(pointer_t ptr, int count) {
    QString line = QString("hdata buffer:0x%1/lines/last_line(-%2)/data\n").arg(ptr, 0, 16).arg(count);
    //qCritical() << "WRITING:" << line;
    auto bytes = m_connection->write(line.toUtf8());
    m_timeoutTimer.start(5000);
    if (bytes != line.toUtf8().count()) {
        errorStringSet("HOVNO");
    }
}

void Lith::onTimeout() {
    m_connection->disconnect();
    statusSet(DISCONNECTED);
    start();
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
            if (m_buffers.count() == 1 && Settings::instance()->lastOpenBufferGet() < 0)
                emit selectedChanged();
            else if (Settings::instance()->lastOpenBufferGet() == m_buffers.count() - 1) {
                setSelectedIndex(Settings::instance()->lastOpenBufferGet());
            }
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
                m_lineMap[ptr] = new BufferLine(nullptr);
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
            m_hotList.insert(ptr, new HotListItem(nullptr));
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
    if (m_selectedIndex >=0 && m_selectedIndex < m_buffers.count())
        return m_buffers[m_selectedIndex];
    return nullptr;
}

void StuffManager::selectedBufferSet(Buffer *b) {
    for (int i = 0; i < m_buffers.count(); i++) {
        if (m_buffers[i] == b) {
            setSelectedIndex(i);
            return;
        }
    }
    setSelectedIndex(-1);
}

int StuffManager::selectedIndex() {
    return m_selectedIndex;
}

void StuffManager::setSelectedIndex(int o) {
    if (m_selectedIndex != o && o < m_buffers.count()) {
        m_selectedIndex = o;
        emit selectedChanged();
        if (selectedBuffer())
            selectedBuffer()->fetchMoreLines();
        if (o >= 0)
            Settings::instance()->lastOpenBufferSet(o);
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
    return QVariant::fromValue(qobject_cast<Buffer*>(m_buffers[index.row()]));
}

void StuffManager::reset() {
    setSelectedIndex(-1);
    beginResetModel();
    for (auto i : m_buffers) {
        i->deleteLater();
    }
    m_buffers.clear();
    m_bufferMap.clear();
    emit buffersChanged();
    endResetModel();
    qCritical() << "=== RESET";
    int lines = 0;
    for (auto i : m_lineMap) {
        if (!i->parent())
            lines++;
        i->deleteLater();
    }
    m_lineMap.clear();
    qCritical() << "There is" << m_lineMap.count() << "orphan lines";
    int hotlist = 0;
    for (auto i : m_hotList) {
        if (!i->parent())
            hotlist++;
        i->deleteLater();
    }
    m_hotList.clear();
    qCritical() << "There is" << m_hotList.count() << "hotlist items";
}

StuffManager::StuffManager() : QAbstractListModel() {
}

ProxyBufferList::ProxyBufferList(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(StuffManager::instance());
    connect(this, &ProxyBufferList::filterWordChanged, this, [this](){
        setFilterFixedString(filterWordGet());
    });
}

bool ProxyBufferList::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    auto index = sourceModel()->index(source_row, 0, source_parent);
    QVariant v = sourceModel()->data(index);
    auto b = qvariant_cast<Buffer*>(v);
    if (b) {
        return b->nameGet().toLower().contains(filterWordGet().toLower());
    }
    return false;
}
