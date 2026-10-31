// Lith
// Copyright (C) 2026 Martin Bříza
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

#include <QtTest/QtTest>

#include "protocol.h"
#include "relayapiclient.h"
#include "weechat.h"

#include <QJsonDocument>
#include <QProcess>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTemporaryDir>

#ifdef Q_OS_LINUX
  #include <csignal>
  #include <sys/prctl.h>
#endif

using WeeChatProtocol::HData;

namespace {
    const QString c_password = QStringLiteral("lith-test-secret");

    quint16 freePort() {
        QTcpServer server;
        server.listen(QHostAddress::LocalHost, 0);
        return server.serverPort();
    }

    QString plain(const QVariant& v) {
        if (v.metaType() == QMetaType::fromType<FormattedString>()) {
            return v.value<FormattedString>().toPlain();
        }
        return v.toString();
    }

    const HData::Item* findItem(const HData& hda, const QString& key, const QVariant& value) {
        for (const auto& item : hda.data) {
            if (plain(item.objects.value(key)) == value.toString()) {
                return &item;
            }
        }
        return nullptr;
    }
}  // namespace

// Collects everything a RelayApiClient reports
class ApiSession : public QObject {
    Q_OBJECT
public:
    explicit ApiSession(QObject* parent = nullptr)
        : QObject(parent) {
        connect(&client, &RelayApiClient::connected, this, [this]() { connects++; });
        connect(&client, &RelayApiClient::disconnected, this, [this]() { disconnects++; });
        connect(&client, &RelayApiClient::errorOccurred, this, [this](const QString& e) { errors.append(e); });
        connect(&client, &RelayApiClient::authenticationFailed, this, [this](const QString& e) { authFailures.append(e); });
        connect(&client, &RelayApiClient::hdataReceived, this, [this](const QString& handler, const HData& hda) {
            received.append({handler, hda});
        });
    }

    QList<HData> hdata(const QString& handler) const {
        QList<HData> result;
        for (const auto& [h, hda] : received) {
            if (h == handler) {
                result.append(hda);
            }
        }
        return result;
    }
    bool has(const QString& handler, const QString& key, const QString& plainValue) const {
        for (const auto& hda : hdata(handler)) {
            for (const auto& item : hda.data) {
                if (plain(item.objects.value(key)) == plainValue) {
                    return true;
                }
            }
        }
        return false;
    }

    RelayApiClient client;
    QList<QPair<QString, HData>> received;
    QStringList errors;
    QStringList authFailures;
    int connects {0};
    int disconnects {0};
};

class RelayIntegrationTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void apiInitialData();
    void apiMatchesBinaryProtocol();
    void apiInputAndLineEvents();
    void apiFetchLines();
    void apiHotlist();
    void apiBufferEvents();
    void apiPing();
    void apiWrongPassword();
    void apiSslAgainstPlainRelay();
    void apiAgainstBinaryRelay();
    void apiReconnectDoesNotLeakStaleSignals();
    void apiHashAlgorithms_data();
    void apiHashAlgorithms();
    void binaryProtocol();

    void nicklistFlattening();
    void dates();

private:
    void connectApi(ApiSession& session, const QString& password = c_password);
    weechat_pointer_t coreBufferId(const ApiSession& session);
    using StopCondition = std::function<bool(const QByteArray& message)>;
    // Reads until expectedMessages were received, or until stop returns true if it's set
    QList<QByteArray> readMessages(QTcpSocket& socket, int expectedMessages, const StopCondition& stop = {});
    QList<QByteArray> binarySession(const QStringList& commands, int expectedMessages, bool handshake = false, const StopCondition& stop = {});
    QMap<QString, HData> binaryHData(const QStringList& commands, int expectedMessages);

    QTemporaryDir m_dir;
    QProcess m_weechat;
    quint16 m_apiPort {0};
    quint16 m_weechatPort {0};
};

void RelayIntegrationTest::initTestCase() {
    auto executable = QStandardPaths::findExecutable(QStringLiteral("weechat-headless"));
    if (executable.isEmpty()) {
        QSKIP("weechat-headless not found");
    }
    QVERIFY(m_dir.isValid());
    m_apiPort = freePort();
    m_weechatPort = freePort();

    QFile config(m_dir.filePath(QStringLiteral("relay.conf")));
    QVERIFY(config.open(QIODevice::WriteOnly));
    // ipv6 must be off, otherwise WeeChat binds an AF_INET6 socket and rejects an IPv4 bind_address
    config.write(QStringLiteral("[network]\nbind_address = \"127.0.0.1\"\nipv6 = off\npassword = \"%1\"\n\n[port]\napi = %2\nweechat = %3\n")
                     .arg(c_password)
                     .arg(m_apiPort)
                     .arg(m_weechatPort)
                     .toUtf8());
    config.close();

    m_weechat.setProgram(executable);
    m_weechat.setArguments({QStringLiteral("--dir"), m_dir.path(), QStringLiteral("-P"), QStringLiteral("relay"), QStringLiteral("--stdout")});
    m_weechat.setStandardInputFile(QProcess::nullDevice());
    m_weechat.setProcessChannelMode(QProcess::MergedChannels);
#ifdef Q_OS_LINUX
    // Don't leave WeeChat running when the test crashes
    m_weechat.setChildProcessModifier([]() { ::prctl(PR_SET_PDEATHSIG, SIGTERM); });
#endif
    m_weechat.start();
    QVERIFY(m_weechat.waitForStarted());

    for (auto port : {m_apiPort, m_weechatPort}) {
        QTRY_VERIFY_WITH_TIMEOUT(
            [port]() {
                QTcpSocket probe;
                probe.connectToHost(QHostAddress::LocalHost, port);
                return probe.waitForConnected(100);
            }(),
            10000
        );
    }
}

void RelayIntegrationTest::cleanupTestCase() {
    if (m_weechat.state() != QProcess::NotRunning) {
        m_weechat.terminate();
        if (!m_weechat.waitForFinished(3000)) {
            m_weechat.kill();
            m_weechat.waitForFinished();
        }
    }
}

void RelayIntegrationTest::connectApi(ApiSession& session, const QString& password) {
    session.client.connectToHost(QStringLiteral("127.0.0.1"), m_apiPort, false, false, password);
    QTRY_VERIFY_WITH_TIMEOUT(session.client.isConnected() || !session.authFailures.isEmpty() || !session.errors.isEmpty(), 10000);
}

weechat_pointer_t RelayIntegrationTest::coreBufferId(const ApiSession& session) {
    for (const auto& hda : session.hdata(RelayApiClient::Handlers::c_buffers)) {
        const auto* item = findItem(hda, QStringLiteral("name"), QStringLiteral("weechat"));
        if (item) {
            return item->pointers.first();
        }
    }
    return 0;
}

QList<QByteArray> RelayIntegrationTest::readMessages(QTcpSocket& socket, int expectedMessages, const StopCondition& stop) {
    QList<QByteArray> messages;
    QByteArray buffer;
    QElapsedTimer timer;
    timer.start();
    while ((stop || messages.size() < expectedMessages) && timer.elapsed() < 10000) {
        if (socket.bytesAvailable() == 0) {
            socket.waitForReadyRead(100);
        }
        buffer.append(socket.read(5 - qMin<qsizetype>(buffer.size(), 5)));
        if (buffer.size() < 5) {
            continue;
        }
        QDataStream header(buffer);
        quint32 length = 0;
        header >> length;
        buffer.append(socket.read(length - buffer.size()));
        if (static_cast<quint32>(buffer.size()) == length) {
            // compression=off is always requested, byte 4 is the compression flag
            messages.append(buffer.mid(5));
            buffer.clear();
            if (stop && stop(messages.last())) {
                break;
            }
        }
    }
    return messages;
}

QList<QByteArray> RelayIntegrationTest::binarySession(const QStringList& commands, int expectedMessages, bool handshake, const StopCondition& stop) {
    QTcpSocket socket;
    socket.connectToHost(QHostAddress::LocalHost, m_weechatPort);
    if (!socket.waitForConnected(3000)) {
        return {};
    }
    QList<QByteArray> messages;
    if (handshake) {
        socket.write("(handleHandshake) handshake password_hash_algo=plain:sha256:sha512:pbkdf2+sha256:pbkdf2+sha512,compression=off\n");
        messages = readMessages(socket, 1);
        if (messages.isEmpty()) {
            return {};
        }
        auto reply = messages.first();
        QDataStream s(&reply, QIODevice::ReadOnly);
        WeeChatProtocol::parse<WeeChatProtocol::PlainString>(s);
        s.skipRawData(3);
        auto htb = WeeChatProtocol::parse<WeeChatProtocol::HashTable>(s);
        auto algo = htb.value(QStringLiteral("password_hash_algo"));
        auto iterations = htb.value(QStringLiteral("password_hash_iterations")).toInt();
        auto salt = QByteArray::fromHex(htb.value(QStringLiteral("nonce")).toLatin1()) + QByteArray::fromHex(Weechat::randomString(16));
        auto hash = Weechat::hashPassword(c_password, algo, salt, iterations).toHex();
        auto init = algo.startsWith(QStringLiteral("pbkdf2"))
                        ? QStringLiteral("init password_hash=%1:%2:%3:%4").arg(algo, QString::fromLatin1(salt.toHex())).arg(iterations).arg(QString::fromLatin1(hash))
                        : QStringLiteral("init password_hash=%1:%2:%3").arg(algo, QString::fromLatin1(salt.toHex()), QString::fromLatin1(hash));
        socket.write(init.toUtf8() + '\n');
    }
    for (const auto& command : commands) {
        socket.write(command.toUtf8() + '\n');
    }
    socket.flush();
    messages.append(readMessages(socket, expectedMessages, stop));
    socket.write("quit\n");
    socket.flush();
    return messages;
}

QMap<QString, HData> RelayIntegrationTest::binaryHData(const QStringList& commands, int expectedMessages) {
    QMap<QString, HData> result;
    for (auto message : binarySession(commands, expectedMessages)) {
        QDataStream s(&message, QIODevice::ReadOnly);
        auto id = WeeChatProtocol::parse<WeeChatProtocol::PlainString>(s);
        QByteArray type(3, 0);
        s.readRawData(type.data(), 3);
        if (type == "hda") {
            result.insert(id, WeeChatProtocol::parse<HData>(s));
        }
    }
    return result;
}

void RelayIntegrationTest::apiInitialData() {
    ApiSession session;
    connectApi(session);
    QVERIFY2(session.client.isConnected(), qPrintable(session.errors.join(QStringLiteral("; ")) + session.authFailures.join(QStringLiteral("; "))));
    QCOMPARE(session.connects, 1);

    QTRY_VERIFY_WITH_TIMEOUT(!session.hdata(RelayApiClient::Handlers::c_hotlistInitialization).isEmpty(), 5000);
    QCOMPARE(session.hdata(RelayApiClient::Handlers::c_buffers).size(), 1);
    QCOMPARE(session.hdata(RelayApiClient::Handlers::c_lines).size(), 1);
    QCOMPARE(session.hdata(RelayApiClient::Handlers::c_nicklist).size(), 1);

    const auto buffers = session.hdata(RelayApiClient::Handlers::c_buffers).first();
    const auto* core = findItem(buffers, QStringLiteral("name"), QStringLiteral("weechat"));
    QVERIFY(core);
    QCOMPARE(core->objects.value(QStringLiteral("number")).toInt(), 1);
    QCOMPARE(core->objects.value(QStringLiteral("local_variables")).value<StringMap>().value(QStringLiteral("plugin")), QStringLiteral("core"));
    QVERIFY(plain(core->objects.value(QStringLiteral("title"))).startsWith(QStringLiteral("WeeChat")));

    // Lines are attributed to their buffer through the path, like the binary protocol's buffer/lines/line/line_data
    const auto lines = session.hdata(RelayApiClient::Handlers::c_lines).first();
    QVERIFY(!lines.data.isEmpty());
    for (const auto& line : lines.data) {
        QCOMPARE(line.pointers.size(), 2);
        QVERIFY(line.objects.value(QStringLiteral("date")).toDateTime().isValid());
    }
}

void RelayIntegrationTest::apiMatchesBinaryProtocol() {
    // A dedicated buffer, the core one gets a new line whenever a relay client connects
    {
        ApiSession setup;
        connectApi(setup);
        QTRY_VERIFY_WITH_TIMEOUT(coreBufferId(setup) != 0, 5000);
        auto core = coreBufferId(setup);
        setup.client.input(core, QStringLiteral("/buffer add lith-compare"));
        setup.client.input(core, QStringLiteral("/eval /command -buffer core.lith-compare core /buffer set title ${color:green}green${color:reset} title"));
        setup.client.input(core, QStringLiteral("/eval /print -buffer lith-compare -tags lith_a,lith_b ${color:cyan}alice\\t${color:red}red${color:reset} and ${color:bold}bold"));
        setup.client.input(core, QStringLiteral("/print -buffer lith-compare second"));
        QTRY_VERIFY_WITH_TIMEOUT(setup.has(QStringLiteral("_buffer_line_added"), QStringLiteral("message"), QStringLiteral("second")), 5000);
    }

    ApiSession session;
    connectApi(session);
    QTRY_VERIFY_WITH_TIMEOUT(!session.hdata(RelayApiClient::Handlers::c_lines).isEmpty(), 5000);
    const auto apiBuffers = session.hdata(RelayApiClient::Handlers::c_buffers).first();
    const auto* apiCompare = findItem(apiBuffers, QStringLiteral("name"), QStringLiteral("lith-compare"));
    QVERIFY(apiCompare);
    session.client.fetchLines(apiCompare->pointers.first(), 3);
    QTRY_VERIFY_WITH_TIMEOUT(!session.hdata(RelayApiClient::Handlers::c_fetchLines).isEmpty(), 5000);

    auto binary = binaryHData(
        {QStringLiteral("init password=%1,compression=off").arg(c_password),
         QStringLiteral("(buffers) hdata buffer:gui_buffers(*) number,name,short_name,hidden,title,local_variables"),
         QStringLiteral("(lines) hdata buffer:gui_buffers(*)/own_lines/last_line(-1)/data"),
         QStringLiteral("(fetch) hdata buffer:gui_buffers(*)/own_lines/last_line(-3)/data")},
        3
    );
    QVERIFY(binary.contains(QStringLiteral("buffers")));
    QVERIFY(binary.contains(QStringLiteral("lines")));
    QVERIFY(binary.contains(QStringLiteral("fetch")));

    QCOMPARE(apiBuffers.data.size(), binary[QStringLiteral("buffers")].data.size());
    for (const auto& expected : binary[QStringLiteral("buffers")].data) {
        auto number = expected.objects.value(QStringLiteral("number")).toInt();
        const auto* actual = findItem(apiBuffers, QStringLiteral("number"), number);
        QVERIFY(actual);
        for (const auto& key : {QStringLiteral("name"), QStringLiteral("short_name"), QStringLiteral("title")}) {
            QCOMPARE(plain(actual->objects.value(key)), plain(expected.objects.value(key)));
            QCOMPARE(actual->objects.value(key).value<FormattedString>().toHtml(), expected.objects.value(key).value<FormattedString>().toHtml());
        }
        QCOMPARE(actual->objects.value(QStringLiteral("hidden")).toBool(), expected.objects.value(QStringLiteral("hidden")).toBool());
        QCOMPARE(
            actual->objects.value(QStringLiteral("local_variables")).value<StringMap>(),
            expected.objects.value(QStringLiteral("local_variables")).value<StringMap>()
        );
    }
    const auto* binaryCompare = findItem(binary[QStringLiteral("buffers")], QStringLiteral("name"), QStringLiteral("lith-compare"));
    QVERIFY(binaryCompare);
    QVERIFY(plain(binaryCompare->objects.value(QStringLiteral("title"))) == QStringLiteral("green title"));

    auto linesOf = [](const HData& hda, weechat_pointer_t buffer) {
        QList<QVariantMap> result;
        for (const auto& item : hda.data) {
            if (item.pointers.first() == buffer) {
                result.append(item.objects);
            }
        }
        return result;
    };
    auto compareLines = [](const QList<QVariantMap>& actualLines, const QList<QVariantMap>& expectedLines) {
        QCOMPARE(actualLines.size(), expectedLines.size());
        for (int i = 0; i < actualLines.size(); i++) {
            const auto& actual = actualLines[i];
            const auto& expected = expectedLines[i];
            for (const auto& key : {QStringLiteral("prefix"), QStringLiteral("message")}) {
                QCOMPARE(plain(actual.value(key)), plain(expected.value(key)));
                QCOMPARE(actual.value(key).value<FormattedString>().toHtml(), expected.value(key).value<FormattedString>().toHtml());
            }
            QCOMPARE(actual.value(QStringLiteral("tags_array")).toStringList(), expected.value(QStringLiteral("tags_array")).toStringList());
            QCOMPARE(actual.value(QStringLiteral("highlight")).toBool(), expected.value(QStringLiteral("highlight")).toBool());
            QCOMPARE(actual.value(QStringLiteral("displayed")).toBool(), expected.value(QStringLiteral("displayed")).toBool());
            auto actualDate = actual.value(QStringLiteral("date")).toDateTime();
            auto expectedDate = expected.value(QStringLiteral("date")).toDateTime();
            QCOMPARE(actualDate.toSecsSinceEpoch(), expectedDate.toSecsSinceEpoch());
            QCOMPARE(actualDate.timeSpec(), expectedDate.timeSpec());
        }
    };

    auto apiInitial = linesOf(session.hdata(RelayApiClient::Handlers::c_lines).first(), apiCompare->pointers.first());
    QCOMPARE(apiInitial.size(), 1);
    QCOMPARE(plain(apiInitial.first().value(QStringLiteral("message"))), QStringLiteral("second"));
    compareLines(apiInitial, linesOf(binary[QStringLiteral("lines")], binaryCompare->pointers.first()));

    auto apiFetched = linesOf(session.hdata(RelayApiClient::Handlers::c_fetchLines).first(), apiCompare->pointers.first());
    QCOMPARE(apiFetched.size(), 2);
    QCOMPARE(plain(apiFetched.last().value(QStringLiteral("prefix"))), QStringLiteral("alice"));
    QCOMPARE(apiFetched.last().value(QStringLiteral("tags_array")).toStringList(), (QStringList {QStringLiteral("lith_a"), QStringLiteral("lith_b")}));
    compareLines(apiFetched, linesOf(binary[QStringLiteral("fetch")], binaryCompare->pointers.first()));
}

void RelayIntegrationTest::apiInputAndLineEvents() {
    ApiSession session;
    connectApi(session);
    QTRY_VERIFY_WITH_TIMEOUT(coreBufferId(session) != 0, 5000);
    auto core = coreBufferId(session);

    QVERIFY(session.client.input(core, QStringLiteral("/print lith-cpp-marker")));
    QTRY_VERIFY_WITH_TIMEOUT(session.has(QStringLiteral("_buffer_line_added"), QStringLiteral("message"), QStringLiteral("lith-cpp-marker")), 5000);

    for (const auto& hda : session.hdata(QStringLiteral("_buffer_line_added"))) {
        for (const auto& item : hda.data) {
            if (plain(item.objects.value(QStringLiteral("message"))) == QStringLiteral("lith-cpp-marker")) {
                QCOMPARE(item.objects.value(QStringLiteral("buffer")).value<weechat_pointer_t>(), core);
                QCOMPARE(item.pointers.size(), 1);
            }
        }
    }
}

void RelayIntegrationTest::apiFetchLines() {
    ApiSession session;
    connectApi(session);
    QTRY_VERIFY_WITH_TIMEOUT(coreBufferId(session) != 0, 5000);
    auto core = coreBufferId(session);

    session.client.input(core, QStringLiteral("/print lith-fetch-1"));
    session.client.input(core, QStringLiteral("/print lith-fetch-2"));
    QTRY_VERIFY_WITH_TIMEOUT(session.has(QStringLiteral("_buffer_line_added"), QStringLiteral("message"), QStringLiteral("lith-fetch-2")), 5000);

    session.client.fetchLines(core, 2);
    QTRY_VERIFY_WITH_TIMEOUT(!session.hdata(RelayApiClient::Handlers::c_fetchLines).isEmpty(), 5000);
    const auto lines = session.hdata(RelayApiClient::Handlers::c_fetchLines).first();
    QCOMPARE(lines.data.size(), 2);
    // Newest first, like the binary protocol's last_line(-N)
    QCOMPARE(plain(lines.data[0].objects.value(QStringLiteral("message"))), QStringLiteral("lith-fetch-2"));
    QCOMPARE(plain(lines.data[1].objects.value(QStringLiteral("message"))), QStringLiteral("lith-fetch-1"));
    QCOMPARE(lines.data[0].pointers.first(), core);
}

void RelayIntegrationTest::apiHotlist() {
    ApiSession session;
    connectApi(session);
    QTRY_VERIFY_WITH_TIMEOUT(!session.hdata(RelayApiClient::Handlers::c_hotlistInitialization).isEmpty(), 5000);

    session.client.requestHotlist();
    QTRY_VERIFY_WITH_TIMEOUT(!session.hdata(RelayApiClient::Handlers::c_hotlist).isEmpty(), 5000);
    for (const auto& item : session.hdata(RelayApiClient::Handlers::c_hotlist).first().data) {
        QCOMPARE(item.objects.value(QStringLiteral("count")).value<QList<int>>().size(), 4);
        QCOMPARE(item.objects.value(QStringLiteral("buffer")).value<weechat_pointer_t>(), item.pointers.first());
    }
}

void RelayIntegrationTest::apiBufferEvents() {
    ApiSession session;
    connectApi(session);
    QTRY_VERIFY_WITH_TIMEOUT(coreBufferId(session) != 0, 5000);
    auto core = coreBufferId(session);

    session.client.input(core, QStringLiteral("/buffer add lith-cpp-buffer"));
    QTRY_VERIFY_WITH_TIMEOUT(session.has(QStringLiteral("_buffer_opened"), QStringLiteral("name"), QStringLiteral("lith-cpp-buffer")), 5000);
    weechat_pointer_t opened = 0;
    for (const auto& hda : session.hdata(QStringLiteral("_buffer_opened"))) {
        if (const auto* item = findItem(hda, QStringLiteral("name"), QStringLiteral("lith-cpp-buffer"))) {
            opened = item->pointers.first();
        }
    }
    QVERIFY(opened != 0);

    session.client.input(core, QStringLiteral("/buffer close lith-cpp-buffer"));
    auto closed = [&session, opened]() {
        for (const auto& hda : session.hdata(QStringLiteral("_buffer_closing"))) {
            if (!hda.data.isEmpty() && hda.data.first().pointers.first() == opened) {
                return true;
            }
        }
        return false;
    };
    QTRY_VERIFY_WITH_TIMEOUT(closed(), 5000);
}

void RelayIntegrationTest::apiPing() {
    ApiSession session;
    connectApi(session);
    for (int i = 0; i < 3; i++) {
        session.client.ping();
        QTest::qWait(300);
    }
    QVERIFY(session.client.isConnected());
    QCOMPARE(session.disconnects, 0);
}

void RelayIntegrationTest::apiWrongPassword() {
    ApiSession session;
    connectApi(session, QStringLiteral("wrong"));
    QCOMPARE(session.authFailures.size(), 1);
    QVERIFY2(session.authFailures.first().contains(QStringLiteral("password"), Qt::CaseInsensitive), qPrintable(session.authFailures.first()));
    QVERIFY(!session.client.isConnected());
    QCOMPARE(session.connects, 0);
}

void RelayIntegrationTest::apiSslAgainstPlainRelay() {
    ApiSession session;
    session.client.setConnectTimeout(2000);
    session.client.connectToHost(QStringLiteral("127.0.0.1"), m_apiPort, true, false, c_password);
    QTRY_VERIFY_WITH_TIMEOUT(!session.errors.isEmpty(), 10000);
    QVERIFY2(session.errors.first().contains(QStringLiteral("disable SSL")), qPrintable(session.errors.first()));
    QCOMPARE(session.disconnects, 1);
    QVERIFY(!session.client.isConnected());
}

void RelayIntegrationTest::apiAgainstBinaryRelay() {
    ApiSession session;
    session.client.setConnectTimeout(2000);
    session.client.connectToHost(QStringLiteral("127.0.0.1"), m_weechatPort, false, false, c_password);
    QTRY_VERIFY_WITH_TIMEOUT(!session.errors.isEmpty(), 10000);
    QVERIFY2(session.errors.first().contains(QStringLiteral("addapi")) || session.errors.first().contains(QStringLiteral("enable SSL")), qPrintable(session.errors.first()));
    QCOMPARE(session.disconnects, 1);
    QVERIFY(!session.client.isConnected());
}

void RelayIntegrationTest::apiReconnectDoesNotLeakStaleSignals() {
    ApiSession session;
    connectApi(session);
    QVERIFY(session.client.isConnected());

    // Replacing a live connection must not report the old one's disconnection
    connectApi(session);
    session.client.reset();
    connectApi(session);
    QTest::qWait(500);
    QVERIFY(session.client.isConnected());
    QCOMPARE(session.disconnects, 0);
    QVERIFY(session.errors.isEmpty());
}

void RelayIntegrationTest::apiHashAlgorithms_data() {
    QTest::addColumn<QString>("algo");
    for (const auto& algo : {"plain", "sha256", "sha512", "pbkdf2+sha256", "pbkdf2+sha512"}) {
        QTest::newRow(algo) << QString::fromLatin1(algo);
    }
}

void RelayIntegrationTest::apiHashAlgorithms() {
    QFETCH(QString, algo);
    auto setAlgo = [this](const QString& value) {
        ApiSession admin;
        connectApi(admin);
        QTRY_VERIFY_WITH_TIMEOUT(coreBufferId(admin) != 0, 5000);
        admin.client.input(coreBufferId(admin), QStringLiteral("/set relay.network.password_hash_algo \"%1\"").arg(value));
        QTest::qWait(300);
    };

    setAlgo(algo);
    ApiSession session;
    connectApi(session);
    auto negotiated = session.client.passwordHashAlgo();
    auto connected = session.client.isConnected();
    setAlgo(QStringLiteral("*"));

    QCOMPARE(negotiated, algo);
    QVERIFY(connected);
}

void RelayIntegrationTest::binaryProtocol() {
    auto messages = binarySession(
        {QStringLiteral("(handleBufferInitialization) hdata buffer:gui_buffers(*) number,name,short_name,hidden,title,local_variables"),
         QStringLiteral("(handleHotlistInitialization) hdata hotlist:gui_hotlist(*)"), QStringLiteral("sync"),
         QStringLiteral("(handleNicklistInitialization) nicklist"), QStringLiteral("input core.weechat /print lith-binary-marker"),
         QStringLiteral("(pong) ping 42")},
        0, true, [](const QByteArray& message) { return message.contains("lith-binary-marker"); }
    );

    QStringList ids;
    bool sawLine = false;
    for (auto message : messages) {
        QDataStream s(&message, QIODevice::ReadOnly);
        auto id = WeeChatProtocol::parse<WeeChatProtocol::PlainString>(s);
        ids.append(id);
        QByteArray type(3, 0);
        s.readRawData(type.data(), 3);
        if (id == QStringLiteral("handleHandshake")) {
            QCOMPARE(type, QByteArray("htb"));
            auto htb = WeeChatProtocol::parse<WeeChatProtocol::HashTable>(s);
            QVERIFY(!htb.value(QStringLiteral("password_hash_algo")).isEmpty());
        } else if (id == QStringLiteral("handleBufferInitialization")) {
            auto hda = WeeChatProtocol::parse<HData>(s);
            QVERIFY(findItem(hda, QStringLiteral("name"), QStringLiteral("weechat")));
        } else if (id == QStringLiteral("_buffer_line_added")) {
            auto hda = WeeChatProtocol::parse<HData>(s);
            for (const auto& item : hda.data) {
                sawLine |= plain(item.objects.value(QStringLiteral("message"))) == QStringLiteral("lith-binary-marker");
            }
        }
    }
    for (const auto& expected : {"handleHandshake", "handleBufferInitialization", "handleHotlistInitialization", "handleNicklistInitialization", "_pong"}) {
        QVERIFY2(ids.contains(QString::fromLatin1(expected)), qPrintable(ids.join(QStringLiteral(", "))));
    }
    QVERIFY(sawLine);
}

void RelayIntegrationTest::nicklistFlattening() {
    // Shape taken from the nicklist_root example in the Relay API documentation
    auto root = QJsonDocument::fromJson(R"({
        "id": 0, "parent_group_id": -1, "name": "root", "color": "", "visible": false,
        "groups": [
            {"id": 11, "parent_group_id": 0, "name": "000|o", "color": "", "visible": true, "groups": [], "nicks": [
                {"id": 21, "parent_group_id": 11, "prefix": "@", "prefix_color": "", "name": "alice", "color": "", "visible": true}
            ]},
            {"id": 12, "parent_group_id": 0, "name": "999|...", "color": "", "visible": true, "groups": [], "nicks": [
                {"id": 22, "parent_group_id": 12, "prefix": " ", "prefix_color": "", "name": "bob", "color": "", "visible": true}
            ]}
        ],
        "nicks": []
    })")
                    .object();
    auto hda = WeeChatProtocol::Api::nickHData(7, root);
    QCOMPARE(hda.data.size(), 4);
    QStringList names;
    for (const auto& item : hda.data) {
        QCOMPARE(item.pointers.first(), weechat_pointer_t {7});
        names.append(plain(item.objects.value(QStringLiteral("name"))));
        bool isGroup = item.objects.value(QStringLiteral("group")).value<char>() != 0;
        QCOMPARE(item.objects.value(QStringLiteral("level")).toInt(), isGroup ? 1 : 0);
    }
    QCOMPARE(names, (QStringList {QStringLiteral("000|o"), QStringLiteral("alice"), QStringLiteral("999|..."), QStringLiteral("bob")}));
    QCOMPARE(plain(hda.data[1].objects.value(QStringLiteral("prefix"))), QStringLiteral("@"));
}

void RelayIntegrationTest::dates() {
    auto date = WeeChatProtocol::Api::dateFromJson(QStringLiteral("2023-12-05T19:46:03.847625Z"));
    QCOMPARE(date.timeSpec(), Qt::LocalTime);
    QCOMPARE(date.toUTC(), QDateTime(QDate(2023, 12, 5), QTime(19, 46, 3, 847), QTimeZone::UTC));
    QCOMPARE(WeeChatProtocol::Api::dateFromJson(QStringLiteral("2023-12-05T19:46:03Z")).toUTC(), QDateTime(QDate(2023, 12, 5), QTime(19, 46, 3), QTimeZone::UTC));
}

QTEST_MAIN(RelayIntegrationTest)
#include "RelayIntegrationTest.moc"
