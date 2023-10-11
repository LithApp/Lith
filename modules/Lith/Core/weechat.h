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
#include "replayproxy.h"
#include "sockethelper.h"
#include "lithcore_export.h"

#include <QSslSocket>
#include <QDataStream>
#include <QTimer>

class Lith;

class LITHCORE_EXPORT Weechat : public QObject {
public:
    Q_OBJECT
public:
    explicit Weechat(BaseNetworkProxy* networkProxy, Lith* lith = nullptr);
    const Lith* lith() const;
    Lith* lith();

    static QByteArray hashPassword(const QString& password, const QString& algo, const QByteArray& salt, int iterations);
    static QByteArray randomString(int length);

    struct MessageNames {
        // these names actually correspond to slot names in Lith
        inline static constexpr ConstLatin1String c_handshake {"handleHandshake"};
        inline static constexpr ConstLatin1String c_requestBuffers {"handleBufferInitialization"};
        inline static constexpr ConstLatin1String c_requestFirstLine {"handleFirstReceivedLine"};
        inline static constexpr ConstLatin1String c_requestHotlist {"handleHotlistInitialization"};
        inline static constexpr ConstLatin1String c_requestNicklist {"handleNicklistInitialization"};
    };
    enum Initialization {
        UNINITIALIZED = 0,
        CONNECTION_OPENED = 1 << 0,
        HANDSHAKE = 1 << 1,
        REQUEST_BUFFERS = 1 << 2,
        REQUEST_FIRST_LINE = 1 << 3,
        REQUEST_HOTLIST = 1 << 4,
        REQUEST_NICKLIST = 1 << 5,
        COMPLETE = CONNECTION_OPENED | HANDSHAKE | REQUEST_BUFFERS | REQUEST_FIRST_LINE | REQUEST_HOTLIST | REQUEST_NICKLIST,
        UNTIL_HANDSHAKE = CONNECTION_OPENED | HANDSHAKE,
    };

public slots:
    void init();

    void start();
    void userRequestedRestart();
    void restart();

    bool input(pointer_t ptr, const QByteArray& data);
    void fetchLines(pointer_t ptr, int count);

private slots:

    void onMessageReceived(QByteArray& data);
    void onPongReceived(qint64 id);

    void requestHotlist();
    void onTimeout();
    void onPingTimeout();

    void onConnectionSettingsChanged();
    QCoro::Task<void> onHandshakeAccepted(StringMap data);

    void onConnected();
    void onDisconnected();
    void onDataReceived(const QByteArray& data);
    void onError(const QString& message);

private:
    Initialization m_initializationStatus {UNINITIALIZED};

    SocketHelper* m_connection {nullptr};
    BaseNetworkProxy* m_networkProxy {nullptr};
    bool m_restarting {false};

    QByteArray m_fetchBuffer;
    qint32 m_bytesRemaining {0};

    QTimer* m_hotlistTimer {new QTimer(this)};
    QTimer* m_timeoutTimer {new QTimer(this)};
    QTimer* m_pingTimer {new QTimer(this)};
    QTimer* m_reconnectTimer {new QTimer(this)};

    qint64 m_messageOrder {0};
    qint64 m_lastReceivedPong {0};
    int m_passwordAttempts {0};

    Lith* m_lith;
};

#endif  // WEECHAT_H
