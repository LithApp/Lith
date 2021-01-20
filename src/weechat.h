// Lith
// Copyright (C) 2020 Martin Bříza
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

#ifndef WEECHAT_H
#define WEECHAT_H

#include "common.h"
#include "settings.h"

#include <QSslSocket>
#include <QDataStream>
#include <QTimer>

class Lith;

class Weechat : public QObject {
public:
    Q_OBJECT
public:
    Weechat(Lith *lith = nullptr);
    Lith *lith();

    static QByteArray hashPassword(const QString &password, const QString &algo, const QByteArray &salt, int iterations);
    static QByteArray randomString(int length);

public slots:
    void init();

    void start();
    void restart();

    bool input(pointer_t ptr, const QString &data);
    void fetchLines(pointer_t ptr, int count);

private slots:
    void onMessageReceived(QByteArray &data);

    void requestHotlist();
    void onTimeout();

    void onConnectionSettingsChanged();
    
    void onHandshakeAccepted(const StringMap &data);

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError e);
    void onSslErrors(const QList<QSslError> &errors);

private:
    struct MessageNames {
        // these names actually correspond to slot names in Lith
        inline static const QString c_handshake { "handleHandshake" };
        inline static const QString c_requestBuffers { "handleBufferInitialization" };
        inline static const QString c_requestFirstLine { "handleFirstReceivedLine" };
        inline static const QString c_requestHotlist { "handleHotlistInitialization" };
        inline static const QString c_requestNicklist { "handleNicklistInitialization" };
    };
    enum Initialization {
        UNINITIALIZED = 0,
        HANDSHAKE = 1 << 0,
        REQUEST_BUFFERS = 1 << 1,
        REQUEST_FIRST_LINE = 1 << 2,
        REQUEST_HOTLIST = 1 << 3,
        REQUEST_NICKLIST = 1 << 4,
        COMPLETE = HANDSHAKE | REQUEST_BUFFERS | REQUEST_FIRST_LINE | REQUEST_HOTLIST | REQUEST_NICKLIST
    } m_initializationStatus { UNINITIALIZED };
    inline static const QMap<QString, Initialization> c_initializationMap {
        { MessageNames::c_handshake, HANDSHAKE },
        { MessageNames::c_requestBuffers, REQUEST_BUFFERS },
        { MessageNames::c_requestFirstLine, REQUEST_FIRST_LINE },
        { MessageNames::c_requestHotlist, REQUEST_HOTLIST },
        { MessageNames::c_requestNicklist, REQUEST_NICKLIST }
    };

    QSslSocket *m_connection { nullptr };
    bool m_restarting { false };

    QByteArray m_fetchBuffer;
    int32_t m_bytesRemaining { 0 };

    QTimer *m_hotlistTimer { new QTimer(this) };
    QTimer *m_reconnectTimer { new QTimer(this) };
    QTimer *m_timeoutTimer { new QTimer(this) };

    int64_t m_messageOrder { 0 };

    Lith *m_lith;
};

#endif // WEECHAT_H
