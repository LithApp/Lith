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

QmlObjectList *Lith::unfilteredBuffers() {
    return m_buffers;
}

Buffer *Lith::selectedBuffer() {
    if (m_selectedBufferIndex >=0 && m_selectedBufferIndex < m_buffers->count())
        return m_buffers->get<Buffer>(m_selectedBufferIndex);
    return nullptr;
}

void Lith::selectedBufferSet(Buffer *b) {
    for (int i = 0; i < m_buffers->count(); i++) {
        auto it = m_buffers->get<Buffer>(i);
        if (it && it == b) {
            selectedBufferIndexSet(i);
            return;
        }
    }
    selectedBufferIndexSet(-1);
}

int Lith::selectedBufferIndex() {
    return m_selectedBufferIndex;
}

void Lith::selectedBufferIndexSet(int index) {
    if (m_selectedBufferIndex != index && index < m_buffers->count()) {
        m_selectedBufferIndex = index;
        emit selectedBufferChanged();
        if (selectedBuffer())
            selectedBuffer()->fetchMoreLines();
        if (index >= 0)
            settingsGet()->lastOpenBufferSet(index);
    }
}

QObject *Lith::getObject(pointer_t ptr, const QString &type, pointer_t parent) {
    if (type.isEmpty()) {
        if (m_bufferMap.contains(ptr))
            return m_bufferMap[ptr];
        if (m_lineMap.contains(ptr))
            return m_lineMap[ptr];
        return nullptr;
    }
    else if (type == "buffer") {
        if (!m_bufferMap.contains(ptr)) {
            Buffer *tmp = new Buffer(this, ptr);
            m_bufferMap[ptr] = tmp;
            m_buffers->append(tmp);
            auto lastOpenBuffer = settingsGet()->lastOpenBufferGet();
            if (m_buffers->count() == 1 && lastOpenBuffer < 0)
                emit selectedBufferChanged();
            else if (lastOpenBuffer == m_buffers->count() - 1) {
                selectedBufferIndexSet(lastOpenBuffer);
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

Weechat *Lith::weechat() {
    return m_weechat;
}

Lith::Lith(QObject *parent)
    : QObject(parent)
    , m_weechat(new Weechat(this))
    , m_buffers(QmlObjectList::create<Buffer>())
    , m_proxyBufferList(new ProxyBufferList(this))
{
    connect(settingsGet(), &Settings::passphraseChanged, this, &Lith::hasPassphraseChanged);
}

bool Lith::hasPassphrase() const {
    return !settingsGet()->passphraseGet().isEmpty();
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

void Lith::resetData() {
    selectedBufferIndexSet(-1);
    for (int i = 0; i < m_buffers->count(); i++) {
        if (m_buffers->get<Buffer>(i))
            m_buffers->get<Buffer>(i)->deleteLater();
    }
    m_buffers->clear();
    m_bufferMap.clear();
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

ProxyBufferList::ProxyBufferList(QObject *parent, QAbstractListModel *parentModel)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(parentModel);
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

Weechat::Weechat(Lith *lith)
    : QObject(lith)
{
    statusSet(UNCONFIGURED);

    m_reconnectTimer.setInterval(1000);
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &Weechat::restart);

    m_timeoutTimer.setInterval(5000);
    m_timeoutTimer.setSingleShot(true);
    connect(&m_timeoutTimer, &QTimer::timeout, this, &Weechat::onTimeout);

    connect(&m_hotlistTimer, &QTimer::timeout, this, &Weechat::requestHotlist);
    m_hotlistTimer.setInterval(1000);
    m_hotlistTimer.setSingleShot(false);

    connect(lith->settingsGet(), &Settings::hostChanged, this, &Weechat::onConnectionSettingsChanged);
    connect(lith->settingsGet(), &Settings::passphraseChanged, this, &Weechat::onConnectionSettingsChanged);
    connect(lith->settingsGet(), &Settings::portChanged, this, &Weechat::onConnectionSettingsChanged);
    connect(lith->settingsGet(), &Settings::encryptedChanged, this, &Weechat::onConnectionSettingsChanged);
    QTimer::singleShot(0, this, &Weechat::onConnectionSettingsChanged);

}

Lith *Weechat::lith() {
    return qobject_cast<Lith*>(parent());
}

void Weechat::start() {
    if (m_connection) {
        m_connection->deleteLater();
        m_connection = nullptr;
    }
    qCritical() << "Connecting";

    statusSet(CONNECTING);
    m_connection = new QSslSocket(this);
    m_connection->ignoreSslErrors({QSslError::UnableToGetLocalIssuerCertificate});
    m_connection->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(m_connection, static_cast<void(QSslSocket::*)(QSslSocket::SocketError)>(&QAbstractSocket::errorOccurred), this, &Weechat::onError);
    connect(m_connection, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), this, &Weechat::onSslErrors);
    connect(m_connection, &QSslSocket::readyRead, this, &Weechat::onReadyRead);
    connect(m_connection, &QSslSocket::connected, this, &Weechat::onConnected);
    connect(m_connection, &QSslSocket::disconnected, this, &Weechat::onDisconnected);

    restart();
}

void Weechat::restart() {
    auto host = lith()->settingsGet()->hostGet();
    auto port = lith()->settingsGet()->portGet();
    auto ssl = lith()->settingsGet()->encryptedGet();
    if (ssl)
        m_connection->connectToHostEncrypted(host, port);
    else
        m_connection->connectToHost(host, port);
    m_reconnectTimer.stop();
}

void Weechat::onConnectionSettingsChanged() {
    auto host = lith()->settingsGet()->hostGet();
    auto pass = lith()->settingsGet()->passphraseGet();
    if (!host.isEmpty() && !pass.isEmpty()) {
        if (m_connection) {
            m_connection->deleteLater();
            m_connection = nullptr;
        }
        QTimer::singleShot(0, this, &Weechat::start);
    }
}

void Weechat::requestHotlist() {
    if (m_connection) {
        m_connection->write("hdata hotlist:gui_hotlist(*)\n");
    }
}

void Weechat::onReadyRead() {
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
        lith()->onMessageReceived(m_fetchBuffer);
        guard = false;
        m_fetchBuffer.clear();
    }

    // if there's still more data left, do one more round
    if (m_connection->bytesAvailable()) {
        onReadyRead();
    }
}

void Weechat::onConnected() {
    qCritical() << "Connected!";
    lith()->resetData();

    statusSet(CONNECTED);
    auto pass = lith()->settingsGet()->passphraseGet();
    m_connection->write(("init password=" + pass + ",compression=off\n").toUtf8());
    m_connection->write("hdata buffer:gui_buffers(*) number,name,hidden,title\n");
    m_connection->write("hdata buffer:gui_buffers(*)/lines/last_line(-1)/data\n");
    m_connection->write("hdata hotlist:gui_hotlist(*)\n");
    m_connection->write("sync\n");
    m_connection->write("nicklist\n");

    m_hotlistTimer.start();
}

void Weechat::onDisconnected() {
    statusSet(DISCONNECTED);

    m_hotlistTimer.stop();
    if (m_reconnectTimer.interval() < 5000)
        m_reconnectTimer.setInterval(m_reconnectTimer.interval() * 2);
    m_reconnectTimer.start();
}

void Weechat::onError(QAbstractSocket::SocketError e) {
    qWarning() << "Error!" << e;
    statusSet(ERROR);
    lith()->errorStringSet(QString("Connection failed: %1").arg(m_connection->errorString()));

    if (m_reconnectTimer.interval() < 5000)
        m_reconnectTimer.setInterval(m_reconnectTimer.interval() * 2);
    m_reconnectTimer.start();
}

void Weechat::onSslErrors(const QList<QSslError> &errors) {
    m_connection->ignoreSslErrors(errors);
    for (auto i : errors) {
        qWarning() << "SSL Error!" << i.errorString();
    }
}

void Weechat::input(pointer_t ptr, const QString &data) {
    QString line = QString("input 0x%1 %2\n").arg(ptr, 0, 16).arg(data);
    //qCritical() << "WRITING:" << line;
    m_connection->write(line.toUtf8());
}

void Weechat::fetchLines(pointer_t ptr, int count) {
    QString line = QString("hdata buffer:0x%1/lines/last_line(-%2)/data\n").arg(ptr, 0, 16).arg(count);
    //qCritical() << "WRITING:" << line;
    auto bytes = m_connection->write(line.toUtf8());
    m_timeoutTimer.start(5000);
    if (bytes != line.toUtf8().count()) {
        qWarning() << "fetchLines: Attempted to write" << line.toUtf8().count() << "but managed to write" << bytes;
    }
}

void Weechat::onTimeout() {
    m_connection->disconnect();
    statusSet(DISCONNECTED);
    start();
}
