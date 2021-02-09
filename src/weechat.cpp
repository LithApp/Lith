// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
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

#include "weechat.h"

#include "lith.h"
#include "protocol.h"

#include <QThread>

#include <QPasswordDigestor>
#include <QCryptographicHash>
#include <QRandomGenerator>

Weechat::Weechat(Lith *lith)
    : QObject(nullptr)
    , m_lith(lith)
{
    connect(&m_connection, &SocketHelper::dataReceived, this, &Weechat::onDataReceived, Qt::QueuedConnection);
    connect(&m_connection, &SocketHelper::connected, this, &Weechat::onConnected, Qt::QueuedConnection);
    connect(&m_connection, &SocketHelper::disconnected, this, &Weechat::onDisconnected, Qt::QueuedConnection);
    connect(&m_connection, &SocketHelper::errorOccurred, this, &Weechat::onError, Qt::QueuedConnection);
}

Lith *Weechat::lith() {
    return m_lith;
}

const QStringList supportedHashAlgos {
    "plain",
    "sha256",
    "sha512",
    "pbkdf2+sha256",
    "pbkdf2+sha512"
};
QByteArray Weechat::hashPassword(const QString &password, const QString &algo, const QByteArray &salt, int iterations) {
    if (algo == "plain") {
        return password.toUtf8();
    }
    else if (algo == "sha256") {
        QCryptographicHash qch(QCryptographicHash::Sha256);
        qch.addData(salt);
        qch.addData(password.toUtf8());
        return qch.result();
    }
    else if (algo == "sha512") {
        QCryptographicHash qch(QCryptographicHash::Sha512);
        qch.addData(salt);
        qch.addData(password.toUtf8());
        return qch.result();
    }
    else if (algo == "pbkdf2+sha256") {
        return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, password.toUtf8(), salt, iterations, 32);
    }
    else if (algo == "pbkdf2+sha512") {
        return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha512, password.toUtf8(), salt, iterations, 64);
    }
    return {};
}

QByteArray Weechat::randomString(int length) {
    QByteArray alphabet { "0123456789ABCDEF"};
    QByteArray result;
    auto rg = QRandomGenerator::securelySeeded();
    for (int i = 0; i < length; i++) {
        result += alphabet[rg.bounded(0, alphabet.count())];
    }
    return result;
}

void Weechat::init() {
    m_timeoutTimer->setInterval(5000);
    m_timeoutTimer->setSingleShot(true);
    //connect(m_timeoutTimer, &QTimer::timeout, this, &Weechat::onTimeout, Qt::QueuedConnection);

    connect(m_hotlistTimer, &QTimer::timeout, this, &Weechat::requestHotlist, Qt::QueuedConnection);
    m_hotlistTimer->setInterval(10000);
    m_hotlistTimer->setSingleShot(false);

    connect(lith()->settingsGet(), &Settings::ready, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(lith()->settingsGet(), &Settings::hostChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(lith()->settingsGet(), &Settings::passphraseChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(lith()->settingsGet(), &Settings::portChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);
    connect(lith()->settingsGet(), &Settings::encryptedChanged, this, &Weechat::onConnectionSettingsChanged, Qt::QueuedConnection);

    onConnectionSettingsChanged();
}

void Weechat::start() {
    m_connection.reset();
    m_restarting = false;
    qCritical() << "Connecting";

    lith()->statusSet(Lith::CONNECTING);

    restart();
}

void Weechat::restart() {
    auto host = lith()->settingsGet()->hostGet();
    auto port = lith()->settingsGet()->portGet();
    auto ssl = lith()->settingsGet()->encryptedGet();
#ifndef Q_OS_WASM
    if (!lith()->settingsGet()->useWebsocketsGet())
        m_connection.connectToTcpSocket(host, port, ssl);
    else // BEWARE
#endif // Q_OS_WASM
        m_connection.connectToWebsocket(host, port, ssl);
    // BEWARE OF THE ELSE ABOVE
}

void Weechat::onConnectionSettingsChanged() {
    auto host = lith()->settingsGet()->hostGet();
    auto pass = lith()->settingsGet()->passphraseGet();
    if (!host.isEmpty() && !pass.isEmpty()) {
        qCritical() << "CONNECTING";
        m_connection.reset();
        if (!m_restarting)
            QTimer::singleShot(50, this, &Weechat::start);
        m_restarting = true;
    }
}

void Weechat::onHandshakeAccepted(const StringMap &data) {
    if (!m_connection.isConnected())
        return;

    auto algo = data["password_hash_algo"];
    auto iterations = data["password_hash_iterations"].toInt();
    auto serverNonce = QByteArray::fromHex(data["nonce"].toLocal8Bit());
    auto clientNonce = QByteArray::fromHex(randomString(16));
    auto pass = lith()->settingsGet()->passphraseGet();

    auto salt = serverNonce + clientNonce;
    auto hash = hashPassword(pass, algo, salt, iterations);

    QString hashString;
    if (algo == "plain")
        hashString = "password=" + pass + ",compression=" + (lith()->settingsGet()->connectionCompressionGet() ? "zlib" : "off");
    else if (algo.startsWith("pbkdf2"))
        hashString = "password_hash=" + algo + ':' + salt.toHex() + ':' + QString("%1").arg(iterations) + ':' + hash.toHex();
    else
        hashString = "password_hash=" + algo + ':' + salt.toHex() + ':' + hash.toHex();

    m_initializationStatus = (Initialization) (m_initializationStatus | HANDSHAKE);

    m_connection.write(("init " + hashString + "\n").toUtf8());
    m_connection.write(QString("(%1) hdata buffer:gui_buffers(*) number,name,short_name,hidden,title,local_variables\n").arg(MessageNames::c_requestBuffers).toUtf8());
    m_connection.write(QString("(%1) hdata buffer:gui_buffers(*)/lines/last_line(-1)/data\n").arg(MessageNames::c_requestFirstLine).toUtf8());
    m_connection.write(QString("(%1) hdata hotlist:gui_hotlist(*)\n").arg(MessageNames::c_requestHotlist).toUtf8());
    m_connection.write("sync\n");
    m_connection.write(QString("(%1) nicklist\n").arg(MessageNames::c_requestNicklist).toUtf8());
}

void Weechat::requestHotlist() {
    if (m_connection.isConnected()) {
        auto msg = QString("(handleHotlist;%1) hdata hotlist:gui_hotlist(*)\n").arg(m_messageOrder++);
        m_connection.write(msg.toUtf8());
    }
}


void Weechat::onConnected() {
    qCritical() << "Connected!";
    QTimer::singleShot(0, lith(), &Lith::resetData);
    lith()->networkErrorStringSet(QString());

    lith()->statusSet(Lith::CONNECTED);
    QString hashAlgos;
    for (auto &i : supportedHashAlgos) {
        if (!hashAlgos.isEmpty())
            hashAlgos.append(":");
        hashAlgos.append(i);
    }

    if (lith()->settingsGet()->handshakeAuthGet()) {
        m_connection.write(QString("(%1) handshake password_hash_algo=%2,compression=%3\n").arg(MessageNames::c_handshake).arg(hashAlgos).arg(lith()->settingsGet()->connectionCompressionGet() ? "zlib" : "off").toUtf8());
    }
    else {
        StringMap data;
        data["password_hash_algo"] = "plain";
        onHandshakeAccepted(data);
    }

    m_hotlistTimer->start();
}

void Weechat::onDisconnected() {
    lith()->statusSet(Lith::DISCONNECTED);

    m_fetchBuffer.clear();
    m_bytesRemaining = 0;
    m_hotlistTimer->stop();
}

void Weechat::onDataReceived(const QByteArray &data) {
    auto dataCopy(data);
    onMessageReceived(dataCopy);
}

void Weechat::onError(const QString &message) {
    lith()->statusSet(Lith::ERROR);
    lith()->networkErrorStringSet("Connection failed: "+ message);
}

bool Weechat::input(pointer_t ptr, const QString &data) {
    // server doesn't reply to input commands directly so no message order here
    auto line = QString("input 0x%1 %2\n").arg(ptr, 0, 16).arg(data);
    //qCritical() << "WRITING:" << line;
    auto message = line.toUtf8();
    if (message.count() == m_connection.write(line.toUtf8()))
        return true;
    return false;
}

void Weechat::fetchLines(pointer_t ptr, int count) {
    auto line = QString("(handleFetchLines;%1) hdata buffer:0x%2/lines/last_line(-%3)/data\n").arg(m_messageOrder++).arg(ptr, 0, 16).arg(count);
    //qCritical() << "WRITING:" << line;
    m_connection.write(line.toUtf8());
    m_timeoutTimer->start(5000);
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
    else if (QString(type) == "htb") {
        Protocol::HashTable htb;
        Protocol::parse(s, htb);

        onHandshakeAccepted(htb.d);
    }
    else {
        qCritical() << "onMessageReceived is not handling type: " << type;
    }

    if (!s.atEnd()) {
        qCritical() << "STREAM WAS NOT AT END!!!";
    }
}

void Weechat::onTimeout() {
    m_connection.reset();
    lith()->statusSet(Lith::DISCONNECTED);
    start();
}
