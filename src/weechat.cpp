#include "weechat.h"

#include "lith.h"
#include "protocol.h"

#include <QThread>

Weechat::Weechat(Lith *lith)
    : QObject(nullptr)
    , m_lith(lith)
{
}

Lith *Weechat::lith() {
    return m_lith;
}

void Weechat::init() {
    m_reconnectTimer->setInterval(1000);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &Weechat::restart, Qt::QueuedConnection);

    m_timeoutTimer->setInterval(5000);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &Weechat::onTimeout, Qt::QueuedConnection);

    connect(m_hotlistTimer, &QTimer::timeout, this, &Weechat::requestHotlist, Qt::QueuedConnection);
    m_hotlistTimer->setInterval(10000);
    m_hotlistTimer->setSingleShot(false);

    connect(lith()->settingsGet(), &Settings::hostChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(lith()->settingsGet(), &Settings::passphraseChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(lith()->settingsGet(), &Settings::portChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(lith()->settingsGet(), &Settings::encryptedChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);

    onConnectionSettingsChanged();
}

void Weechat::start() {
    if (m_connection) {
        m_connection->deleteLater();
        m_connection = nullptr;
    }
    qCritical() << "Connecting";

    lith()->statusSet(Lith::CONNECTING);
    m_connection = new QSslSocket(this);
    m_connection->ignoreSslErrors({QSslError::UnableToGetLocalIssuerCertificate});
    m_connection->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_connection, static_cast<void(QSslSocket::*)(QSslSocket::SocketError)>(&QAbstractSocket::errorOccurred), this, &Weechat::onError, Qt::QueuedConnection);
#else
    connect(m_connection, static_cast<void(QSslSocket::*)(QSslSocket::SocketError)>(&QAbstractSocket::error), this, &Weechat::onError, Qt::QueuedConnection);
#endif
    connect(m_connection, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), this, &Weechat::onSslErrors, Qt::QueuedConnection);
    connect(m_connection, &QSslSocket::readyRead, this, &Weechat::onReadyRead, Qt::QueuedConnection);
    connect(m_connection, &QSslSocket::connected, this, &Weechat::onConnected, Qt::QueuedConnection);
    connect(m_connection, &QSslSocket::disconnected, this, &Weechat::onDisconnected, Qt::QueuedConnection);

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
    m_reconnectTimer->stop();
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
        auto msg = QString("(handleHotlist;%1) hdata hotlist:gui_hotlist(*)\n").arg(m_messageOrder++);
        m_connection->write(msg.toUtf8());
    }
}

void Weechat::onReadyRead() {
    m_timeoutTimer->stop();

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
        auto header = m_connection->read(5);
        QDataStream s(&header, QIODevice::ReadOnly);
        s >> m_bytesRemaining >> compressed;
        m_bytesRemaining -= 5;
        m_fetchBuffer.clear();
    }

    // continue in whatever message came before (be it from a previous readyRead or this one
    if (m_bytesRemaining > 0) {
        auto cache = m_connection->read(m_bytesRemaining);
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
    if (m_connection && m_connection->bytesAvailable()) {
        onReadyRead();
    }
}

void Weechat::onConnected() {
    qCritical() << "Connected!";
    lith()->resetData();

    lith()->statusSet(Lith::CONNECTED);
    auto pass = lith()->settingsGet()->passphraseGet();
    m_connection->write(("init password=" + pass + ",compression=off\n").toUtf8());
    m_connection->write(QString("(%1) hdata buffer:gui_buffers(*) number,name,short_name,hidden,title\n").arg(MessageNames::c_requestBuffers).toUtf8());
    m_connection->write(QString("(%1) hdata buffer:gui_buffers(*)/lines/last_line(-1)/data\n").arg(MessageNames::c_requestFirstLine).toUtf8());
    m_connection->write(QString("(%1) hdata hotlist:gui_hotlist(*)\n").arg(MessageNames::c_requestHotlist).toUtf8());
    m_connection->write("sync\n");
    m_connection->write(QString("(%1) nicklist\n").arg(MessageNames::c_requestNicklist).toUtf8());

    m_hotlistTimer->start();
}

void Weechat::onDisconnected() {
    lith()->statusSet(Lith::DISCONNECTED);

    m_hotlistTimer->stop();
    if (m_reconnectTimer->interval() < 5000)
        m_reconnectTimer->setInterval(m_reconnectTimer->interval() * 2);
    m_reconnectTimer->start();
}

void Weechat::onError(QAbstractSocket::SocketError e) {
    qWarning() << "Error!" << e;
    lith()->statusSet(Lith::ERROR);
    lith()->errorStringSet(QString("Connection failed: %1").arg(m_connection->errorString()));

    if (m_reconnectTimer->interval() < 5000)
        m_reconnectTimer->setInterval(m_reconnectTimer->interval() * 2);
    m_reconnectTimer->start();
}

void Weechat::onSslErrors(const QList<QSslError> &errors) {
    m_connection->ignoreSslErrors(errors);
    for (auto i : errors) {
        qWarning() << "SSL Error!" << i.errorString();
    }
}

void Weechat::input(pointer_t ptr, const QString &data) {
    // server doesn't reply to input commands directly so no message order here
    auto line = QString("input 0x%1 %2\n").arg(ptr, 0, 16).arg(data);
    //qCritical() << "WRITING:" << line;
    m_connection->write(line.toUtf8());
}

void Weechat::fetchLines(pointer_t ptr, int count) {
    auto line = QString("(handleFetchLines;%1) hdata buffer:0x%2/lines/last_line(-%3)/data\n").arg(m_messageOrder++).arg(ptr, 0, 16).arg(count);
    //qCritical() << "WRITING:" << line;
    auto bytes = m_connection->write(line.toUtf8());
    m_timeoutTimer->start(5000);
    if (bytes != line.toUtf8().count()) {
        qWarning() << "fetchLines: Attempted to write" << line.toUtf8().count() << "but managed to write" << bytes;
    }
}

void Weechat::onMessageReceived(QByteArray &data) {
    //qCritical() << "Message!" << data;
    QDataStream s(&data, QIODevice::ReadOnly);

    Protocol::String id;
    Protocol::parse(s, id);

    char type[4] = { 0 };
    s.readRawData(type, 3);

    if (QString(type) == "hda") {
        Protocol::HData *hda = new Protocol::HData();
        Protocol::parse(s, *hda);

        if (c_initializationMap.contains(id.d)) {
            // wtf, why can't I write this as |= ?
            m_initializationStatus = (Initialization) (m_initializationStatus | c_initializationMap.value(id.d, UNINITIALIZED));
            if (!QMetaObject::invokeMethod(Lith::instance(), id.d.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(Protocol::HData*, hda))) {
                qWarning() << "Possible unhandled message:" << id.d;
                delete hda;
            }
        }
        else {
            auto name = id.d.split(";").first();
            if (!QMetaObject::invokeMethod(Lith::instance(), name.toStdString().c_str(), Qt::QueuedConnection, Q_ARG(Protocol::HData*, hda))) {
                qWarning() << "Possible unhandled message:" << name;
                delete hda;
            }
        }
    }
    else {
        qCritical() << "onMessageReceived is not handling type: " << type;
    }

    if (!s.atEnd()) {
        qCritical() << "STREAM WAS NOT AT END!!!";
    }
}

void Weechat::onTimeout() {
    m_connection->disconnect();
    lith()->statusSet(Lith::DISCONNECTED);
    start();
}
