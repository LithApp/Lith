#include "relayapiclient.h"
#include "weechat.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslError>
#include <QUrl>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketHandshakeOptions>

#ifndef __EMSCRIPTEN__
namespace {
    QList<QSslError> ignorableSslErrors(const QList<QSslError>& errors) {
        QList<QSslError> result;
        for (const auto& error : errors) {
            switch (error.error()) {
                case QSslError::SelfSignedCertificate:
                case QSslError::SelfSignedCertificateInChain:
                case QSslError::HostNameMismatch:
                    result.append(error);
                    break;
                default:
                    break;
            }
        }
        return result;
    }
}  // namespace
#endif  // __EMSCRIPTEN__

RelayApiClient::RelayApiClient(QObject* parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this)) {
    m_connectTimer->setSingleShot(true);
    m_connectTimer->setInterval(15000);
    connect(m_connectTimer, &QTimer::timeout, this, &RelayApiClient::onConnectTimeout);
}

RelayApiClient::~RelayApiClient() {
    reset();
}

bool RelayApiClient::isConnected() const {
    return m_connected;
}

QString RelayApiClient::passwordHashAlgo() const {
    return m_hashAlgo;
}

void RelayApiClient::setConnectTimeout(int msec) {
    m_connectTimer->setInterval(msec);
}

QString RelayApiClient::authorization() const {
    if (m_hashAlgo == QStringLiteral("plain")) {
        return QStringLiteral("plain:") + m_password;
    }
    // The timestamp is the salt, so a fresh hash is needed for every authenticated request
    auto timestamp = QByteArray::number(QDateTime::currentSecsSinceEpoch());
    auto hash = QString::fromLatin1(Weechat::hashPassword(m_password, m_hashAlgo, timestamp, m_hashIterations).toHex());
    if (m_hashAlgo.startsWith(QStringLiteral("pbkdf2"))) {
        return QStringLiteral("hash:%1:%2:%3:%4").arg(m_hashAlgo, QString::fromLatin1(timestamp)).arg(m_hashIterations).arg(hash);
    }
    return QStringLiteral("hash:%1:%2:%3").arg(m_hashAlgo, QString::fromLatin1(timestamp), hash);
}

void RelayApiClient::connectToHost(
    const QString& host, int port, bool encrypted, bool allowSelfSignedCertificates, const QString& password
) {
    reset();
    m_host = host;
    m_port = port;
    m_encrypted = encrypted;
    m_allowSelfSignedCertificates = allowSelfSignedCertificates;
    m_password = password;
    m_hashAlgo.clear();
    m_hashIterations = 0;

    QJsonObject body {
        {QStringLiteral("password_hash_algo"),
         QJsonArray {
             QStringLiteral("plain"), QStringLiteral("sha256"), QStringLiteral("sha512"), QStringLiteral("pbkdf2+sha256"),
             QStringLiteral("pbkdf2+sha512")
         }}
    };
    auto request = httpRequest(QStringLiteral("/api/handshake"));
    emit logMessage(QStringLiteral("Relay API: handshake with %1").arg(request.url().toString()));
    m_connectTimer->start();
    track(m_network->post(request, QJsonDocument(body).toJson(QJsonDocument::Compact)));
    connect(m_reply, &QNetworkReply::finished, this, &RelayApiClient::onHandshakeFinished);
}

void RelayApiClient::reset() {
    m_connectTimer->stop();
    if (m_reply) {
        m_reply->disconnect(this);
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    // Signals are disconnected first so a dying socket can't report its disconnection as the current one's
    if (m_socket) {
        m_socket->disconnect(this);
        m_socket->abort();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
    m_connected = false;
    m_pingPending = false;
    m_pendingRequests.clear();
}

QNetworkRequest RelayApiClient::httpRequest(const QString& path) const {
    QUrl url;
    url.setScheme(m_encrypted ? QStringLiteral("https") : QStringLiteral("http"));
    url.setHost(m_host);
    url.setPort(m_port);
    url.setPath(path);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    if (!m_hashAlgo.isEmpty()) {
        request.setRawHeader("Authorization", QByteArrayLiteral("Basic ") + authorization().toUtf8().toBase64());
    }
    return request;
}

QNetworkReply* RelayApiClient::track(QNetworkReply* reply) {
    m_reply = reply;
#ifndef __EMSCRIPTEN__
    connect(reply, &QNetworkReply::sslErrors, this, [this, reply](const QList<QSslError>& errors) {
        for (const auto& error : errors) {
            emit logMessage(QStringLiteral("SSL error: %1").arg(error.errorString()));
        }
        if (m_allowSelfSignedCertificates) {
            reply->ignoreSslErrors(ignorableSslErrors(errors));
        }
    });
#endif  // __EMSCRIPTEN__
    return reply;
}

QString RelayApiClient::serverDescription() const {
    return QStringLiteral("%1:%2 (%3)").arg(m_host).arg(m_port).arg(m_encrypted ? QStringLiteral("SSL") : QStringLiteral("no SSL"));
}

QString RelayApiClient::connectionHint() const {
    if (m_encrypted) {
        return tr("if the relay doesn't use TLS (no \"tls.\" prefix in /relay listfull), disable SSL in Lith");
    }
    return tr("if the relay uses TLS (\"tls.\" prefix in /relay listfull), enable SSL in Lith");
}

void RelayApiClient::onConnectTimeout() {
    QString stage;
    if (m_socket) {
        stage = QStringLiteral("opening the websocket");
    } else if (!m_hashAlgo.isEmpty()) {
        stage = QStringLiteral("authentication");
    } else {
        stage = QStringLiteral("handshake");
    }
    fail(tr("WeeChat at %1 didn't respond during %2 within %3 s, %4")
             .arg(serverDescription(), stage)
             .arg(m_connectTimer->interval() / 1000)
             .arg(connectionHint()));
}

void RelayApiClient::onHandshakeFinished() {
    auto* reply = std::exchange(m_reply, nullptr);
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        // A binary weechat relay closes the connection on an HTTP request, other servers answer 404
        if (reply->error() == QNetworkReply::RemoteHostClosedError || status == 404) {
            fail(tr("Relay API handshake with %1 failed: %2. Is the relay an \"api\" one (/relay addapi), not \"weechat\"? Also, %3")
                     .arg(serverDescription(), reply->errorString(), connectionHint()));
        } else {
            fail(tr("Relay API handshake with %1 failed: %2").arg(serverDescription(), reply->errorString()));
        }
        return;
    }

    auto body = QJsonDocument::fromJson(reply->readAll()).object();
    auto algo = body.value(QStringLiteral("password_hash_algo")).toString();
    if (algo.isEmpty()) {
        emit authenticationFailed(tr("WeeChat doesn't allow any password hash algorithm supported by Lith"));
        return;
    }
    if (body.value(QStringLiteral("totp")).toBool()) {
        emit authenticationFailed(tr("WeeChat requires TOTP (relay.network.totp_secret), Lith doesn't support it yet"));
        return;
    }
    m_hashAlgo = algo;
    m_hashIterations = body.value(QStringLiteral("password_hash_iterations")).toInt();
    emit logMessage(QStringLiteral("Relay API: handshake negotiated %1, authenticating").arg(m_hashAlgo));

    // Checking credentials over HTTP first, a failed websocket upgrade doesn't tell why it failed
    track(m_network->get(httpRequest(QStringLiteral("/api/version"))));
    connect(m_reply, &QNetworkReply::finished, this, &RelayApiClient::onVersionFinished);
}

void RelayApiClient::onVersionFinished() {
    auto* reply = std::exchange(m_reply, nullptr);
    reply->deleteLater();
    auto body = QJsonDocument::fromJson(reply->readAll()).object();
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
        auto error = body.value(QStringLiteral("error")).toString(reply->errorString());
        if (error == QStringLiteral("Invalid timestamp")) {
            error = tr("%1: the clocks of this device and the WeeChat server differ by more than relay.network.time_window "
                       "seconds (default 5), synchronize the server's clock (NTP) or increase that option")
                        .arg(error);
        }
        emit authenticationFailed(error);
        return;
    }
    if (reply->error() != QNetworkReply::NoError) {
        fail(tr("Relay API authentication with %1 failed: %2").arg(serverDescription(), reply->errorString()));
        return;
    }
    emit logMessage(QStringLiteral("Relay API: authenticated to WeeChat %1 (relay API %2)")
                        .arg(body.value(QStringLiteral("weechat_version")).toString(),
                             body.value(QStringLiteral("relay_api_version")).toString()));
    openWebSocket();
}

void RelayApiClient::openWebSocket() {
    QUrl url;
    url.setScheme(m_encrypted ? QStringLiteral("wss") : QStringLiteral("ws"));
    url.setHost(m_host);
    url.setPort(m_port);
    url.setPath(QStringLiteral("/api"));
    QNetworkRequest request(url);
    QWebSocketHandshakeOptions options;
#ifdef __EMSCRIPTEN__
    // Browsers can't set the Authorization header on websockets
    auto token = authorization().toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    options.setSubprotocols({QStringLiteral("api.weechat"), QStringLiteral("base64url.bearer.authorization.weechat.") + QString::fromLatin1(token)});
#else
    request.setRawHeader("Authorization", QByteArrayLiteral("Basic ") + authorization().toUtf8().toBase64());
#endif  // __EMSCRIPTEN__

    m_socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    connect(m_socket, &QWebSocket::connected, this, &RelayApiClient::onSocketConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &RelayApiClient::onSocketDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &RelayApiClient::onTextMessageReceived);
    connect(m_socket, &QWebSocket::errorOccurred, this, [this]() { fail(m_socket->errorString()); });
#ifndef __EMSCRIPTEN__
    connect(m_socket, &QWebSocket::sslErrors, this, [this](const QList<QSslError>& errors) {
        for (const auto& error : errors) {
            emit logMessage(QStringLiteral("SSL error: %1").arg(error.errorString()));
        }
        if (m_allowSelfSignedCertificates) {
            m_socket->ignoreSslErrors(ignorableSslErrors(errors));
        }
    });
#endif  // __EMSCRIPTEN__

    emit logMessage(QStringLiteral("Relay API: opening websocket %1").arg(url.toString()));
    m_socket->open(request, options);
}

void RelayApiClient::onSocketConnected() {
    m_connectTimer->stop();
    emit logMessage(QStringLiteral("Relay API: websocket open, requesting buffers and starting synchronization"));
    m_connected = true;
    emit connected();

    // Sent at once so no event gets lost between the initial fetch and the start of the synchronization
    send(QJsonArray {
        makeRequest(
            QStringLiteral("GET /api/buffers?lines=-1&lines_free=-1&nicks=true&colors=weechat"), {},
            [this](const QString&, const QJsonValue& body) { handleInitialBuffers(body.toArray()); }
        ),
        makeRequest(
            QStringLiteral("GET /api/hotlist"), {},
            [this](const QString&, const QJsonValue& body) {
                emit hdataReceived(Handlers::c_hotlistInitialization, WeeChatProtocol::Api::hotlistHData(body.toArray()));
            }
        ),
        makeRequest(QStringLiteral("POST /api/sync"), {{QStringLiteral("colors"), QStringLiteral("weechat")}, {QStringLiteral("input"), false}}),
    });
}

void RelayApiClient::onSocketDisconnected() {
    emit logMessage(QStringLiteral("Relay API: websocket closed (code %1%2)")
                        .arg(m_socket->closeCode())
                        .arg(m_socket->closeReason().isEmpty() ? QString() : QStringLiteral(", ") + m_socket->closeReason()));
    fail(QString());
}

void RelayApiClient::fail(const QString& message) {
    reset();
    if (!message.isEmpty()) {
        emit errorOccurred(message);
    }
    emit disconnected();
}

void RelayApiClient::onTextMessageReceived(const QString& message) {
    auto doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isArray()) {
        const auto array = doc.array();
        for (const auto& i : array) {
            handleMessage(i.toObject());
        }
    } else {
        handleMessage(doc.object());
    }
}

void RelayApiClient::handleMessage(const QJsonObject& msg) {
    auto code = msg.value(QStringLiteral("code")).toInt(-1);
    if (code == 0) {
        handleEvent(msg);
        return;
    }
    auto callback = m_pendingRequests.take(msg.value(QStringLiteral("request_id")).toString());
    if (code < 200 || code >= 300) {
        emit logMessage(QStringLiteral("Relay API request \"%1\" failed with %2: %3")
                            .arg(msg.value(QStringLiteral("request")).toString())
                            .arg(code)
                            .arg(QString::fromUtf8(QJsonDocument(msg.value(QStringLiteral("body")).toObject()).toJson(QJsonDocument::Compact))));
        return;
    }
    if (callback) {
        callback(msg.value(QStringLiteral("body_type")).toString(), msg.value(QStringLiteral("body")));
    }
}

void RelayApiClient::handleInitialBuffers(const QJsonArray& buffers) {
    WeeChatProtocol::HData lines;
    WeeChatProtocol::HData nicks;
    for (const auto& i : buffers) {
        auto buffer = i.toObject();
        auto ptr = static_cast<weechat_pointer_t>(buffer.value(QStringLiteral("id")).toInteger());
        lines.data.append(WeeChatProtocol::Api::lineHData(ptr, buffer.value(QStringLiteral("lines")).toArray(), false).data);
        nicks.data.append(WeeChatProtocol::Api::nickHData(ptr, buffer.value(QStringLiteral("nicklist_root")).toObject()).data);
    }
    emit logMessage(QStringLiteral("Relay API: received %1 buffers, %2 lines, %3 nicklist entries")
                        .arg(buffers.size())
                        .arg(lines.data.size())
                        .arg(nicks.data.size()));
    emit hdataReceived(Handlers::c_buffers, WeeChatProtocol::Api::bufferHData(buffers));
    emit hdataReceived(Handlers::c_lines, lines);
    emit hdataReceived(Handlers::c_nicklist, nicks);
}

void RelayApiClient::handleEvent(const QJsonObject& msg) {
    using namespace WeeChatProtocol;

    // Events whose body is the buffer, they map 1:1 to the binary protocol's _buffer_* messages
    static const QStringList bufferEvents {
        QStringLiteral("buffer_type_changed"),     QStringLiteral("buffer_moved"),           QStringLiteral("buffer_merged"),
        QStringLiteral("buffer_unmerged"),         QStringLiteral("buffer_hidden"),          QStringLiteral("buffer_unhidden"),
        QStringLiteral("buffer_renamed"),          QStringLiteral("buffer_title_changed"),   QStringLiteral("buffer_localvar_added"),
        QStringLiteral("buffer_localvar_changed"), QStringLiteral("buffer_localvar_removed"), QStringLiteral("buffer_cleared"),
    };
    static const QMap<QString, QPair<bool, char>> nicklistEvents {
        {QStringLiteral("nicklist_nick_added"),     {false, '+'}},
        {QStringLiteral("nicklist_nick_changed"),   {false, '^'}},
        {QStringLiteral("nicklist_nick_removing"),  {false, '-'}},
        {QStringLiteral("nicklist_group_added"),    {true, '+'}},
        {QStringLiteral("nicklist_group_changed"),  {true, '^'}},
        {QStringLiteral("nicklist_group_removing"), {true, '-'}},
    };

    auto event = msg.value(QStringLiteral("event_name")).toString();
    auto bufferPtr = static_cast<weechat_pointer_t>(msg.value(QStringLiteral("buffer_id")).toInteger());
    auto body = msg.value(QStringLiteral("body")).toObject();

    if (event == QStringLiteral("buffer_opened")) {
        emit hdataReceived(QStringLiteral("_buffer_opened"), Api::bufferHData(body));
        emit hdataReceived(Handlers::c_lines, Api::lineHData(bufferPtr, body.value(QStringLiteral("lines")).toArray(), false));
        emit hdataReceived(QStringLiteral("_nicklist"), Api::nickHData(bufferPtr, body.value(QStringLiteral("nicklist_root")).toObject()));
    } else if (bufferEvents.contains(event)) {
        emit hdataReceived(QLatin1Char('_') + event, Api::bufferHData(body));
    } else if (event == QStringLiteral("buffer_closing") || event == QStringLiteral("buffer_closed")) {
        HData hda;
        hda.data.append(HData::Item {{bufferPtr}, {}});
        emit hdataReceived(QStringLiteral("_buffer_closing"), hda);
    } else if (event == QStringLiteral("buffer_line_added")) {
        emit hdataReceived(QStringLiteral("_buffer_line_added"), Api::lineHData(bufferPtr, body, true));
    } else if (nicklistEvents.contains(event)) {
        auto [isGroup, op] = nicklistEvents.value(event);
        emit hdataReceived(QStringLiteral("_nicklist_diff"), Api::nickDiffHData(bufferPtr, body, isGroup, op));
    } else if (event == QStringLiteral("upgrade") || event == QStringLiteral("upgrade_ended") || event == QStringLiteral("quit")) {
        emit logMessage(QStringLiteral("WeeChat event: %1").arg(event));
    }
}

QJsonObject RelayApiClient::makeRequest(const QString& request, const QJsonObject& body, Callback callback) {
    QJsonObject frame {{QStringLiteral("request"), request}};
    if (!body.isEmpty()) {
        frame[QStringLiteral("body")] = body;
    }
    if (callback) {
        auto id = QString::number(m_requestCounter++);
        frame[QStringLiteral("request_id")] = id;
        m_pendingRequests.insert(id, std::move(callback));
    }
    return frame;
}

void RelayApiClient::send(const QJsonValue& requests) {
    if (!m_socket) {
        return;
    }
    auto text = QString::fromUtf8(
        requests.isArray() ? QJsonDocument(requests.toArray()).toJson(QJsonDocument::Compact)
                           : QJsonDocument(requests.toObject()).toJson(QJsonDocument::Compact)
    );
    m_socket->sendTextMessage(text);
}

bool RelayApiClient::input(weechat_pointer_t buffer, const QString& command) {
    if (!m_connected) {
        return false;
    }
    send(makeRequest(
        QStringLiteral("POST /api/input"),
        {{QStringLiteral("buffer_id"), static_cast<qint64>(buffer)}, {QStringLiteral("command"), command}}
    ));
    return true;
}

void RelayApiClient::fetchLines(weechat_pointer_t buffer, int count) {
    if (!m_connected) {
        return;
    }
    send(makeRequest(
        QStringLiteral("GET /api/buffers/%1/lines?lines=-%2&colors=weechat").arg(buffer).arg(count), {},
        [this, buffer](const QString&, const QJsonValue& body) {
            emit hdataReceived(Handlers::c_fetchLines, WeeChatProtocol::Api::lineHData(buffer, body.toArray(), false));
        }
    ));
}

void RelayApiClient::requestHotlist() {
    if (!m_connected) {
        return;
    }
    send(makeRequest(QStringLiteral("GET /api/hotlist"), {}, [this](const QString&, const QJsonValue& body) {
        emit hdataReceived(Handlers::c_hotlist, WeeChatProtocol::Api::hotlistHData(body.toArray()));
    }));
}

void RelayApiClient::ping() {
    if (!m_connected) {
        return;
    }
    if (m_pingPending) {
        fail(QStringLiteral("WeeChat didn't respond to ping in time"));
        return;
    }
    m_pingPending = true;
    send(makeRequest(QStringLiteral("POST /api/ping"), {{QStringLiteral("data"), QStringLiteral("lith")}}, [this](const QString&, const QJsonValue&) {
        m_pingPending = false;
    }));
}
