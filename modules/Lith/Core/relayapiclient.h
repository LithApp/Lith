#ifndef RELAYAPICLIENT_H
#define RELAYAPICLIENT_H

#include "common.h"
#include "protocol.h"
#include "lithcore_export.h"

#include <QHash>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QTimer>

#include <functional>

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QWebSocket;

// Client for the WeeChat Relay API (https://weechat.org/files/doc/weechat/devel/weechat_relay_api.en.html).
// Everything received is reported as the same hdata the binary weechat protocol produces, keyed by the Lith slot handling it.
class LITHCORE_EXPORT RelayApiClient : public QObject {
    Q_OBJECT
public:
    struct Handlers {
        inline static constexpr ConstLatin1String c_buffers {"handleBufferInitialization"};
        inline static constexpr ConstLatin1String c_lines {"handleFirstReceivedLine"};
        inline static constexpr ConstLatin1String c_hotlistInitialization {"handleHotlistInitialization"};
        inline static constexpr ConstLatin1String c_nicklist {"handleNicklistInitialization"};
        inline static constexpr ConstLatin1String c_fetchLines {"handleFetchLines"};
        inline static constexpr ConstLatin1String c_hotlist {"handleHotlist"};
    };

    explicit RelayApiClient(QObject* parent = nullptr);
    ~RelayApiClient() override;

    bool isConnected() const;
    QString passwordHashAlgo() const;
    // Limits the whole sequence of handshake, authentication and opening the websocket
    void setConnectTimeout(int msec);

    QString authorization() const;

public slots:
    void connectToHost(const QString& host, int port, bool encrypted, bool allowSelfSignedCertificates, const QString& password);
    void reset();

    bool input(weechat_pointer_t buffer, const QString& command);
    void fetchLines(weechat_pointer_t buffer, int count);
    void requestHotlist();
    void ping();

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString& message);
    void authenticationFailed(const QString& message);
    void hdataReceived(const QString& handler, const WeeChatProtocol::HData& hda);
    void logMessage(const QString& message);

private:
    using Callback = std::function<void(const QString& bodyType, const QJsonValue& body)>;

    QNetworkRequest httpRequest(const QString& path) const;
    QNetworkReply* track(QNetworkReply* reply);
    QString serverDescription() const;
    QString connectionHint() const;
    void onConnectTimeout();
    void onHandshakeFinished();
    void onVersionFinished();
    void openWebSocket();

    void onSocketConnected();
    void onSocketDisconnected();
    void onTextMessageReceived(const QString& message);
    void handleMessage(const QJsonObject& msg);
    void handleEvent(const QJsonObject& msg);
    void handleInitialBuffers(const QJsonArray& buffers);

    QJsonObject makeRequest(const QString& request, const QJsonObject& body = {}, Callback callback = {});
    void send(const QJsonValue& requests);
    void fail(const QString& message);

    QNetworkAccessManager* m_network {nullptr};
    QNetworkReply* m_reply {nullptr};
    QWebSocket* m_socket {nullptr};
    QTimer* m_connectTimer {new QTimer(this)};

    QString m_host;
    int m_port {0};
    bool m_encrypted {false};
    bool m_allowSelfSignedCertificates {false};
    QString m_password;
    QString m_hashAlgo;
    int m_hashIterations {0};

    bool m_connected {false};
    bool m_pingPending {false};
    qint64 m_requestCounter {0};
    QHash<QString, Callback> m_pendingRequests;
};

#endif  // RELAYAPICLIENT_H
