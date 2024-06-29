// Lith
// Copyright (C) 2024 Martin Bříza
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

class ProtocolTest : public QObject {
    Q_OBJECT
private slots:
    void char1_data() {
        QTest::addColumn<char>("input");
        QTest::addColumn<WeeChatProtocol::Char>("output");
        for (int i = 1; i < CHAR_MAX; i++) {
            QTest::newRow(QString("Char %1").arg(i).toLocal8Bit().data()) << static_cast<char>(i) << static_cast<WeeChatProtocol::Char>(i);
        }
    }
    void char1() {
        QFETCH(char, input);
        QFETCH(WeeChatProtocol::Char, output);

        QByteArray data(1, input);
        QDataStream ds(&data, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::Char>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
private slots:
    void integer1_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<WeeChatProtocol::Integer>("output");
        QTest::newRow("basic") << QByteArray::fromHex("00000041") << WeeChatProtocol::Integer(65);
        QTest::newRow("min") << QByteArray::fromHex("80000000") << WeeChatProtocol::Integer(INT_MIN);
        QTest::newRow("max") << QByteArray::fromHex("7FFFFFFF") << WeeChatProtocol::Integer(INT_MAX);
        QTest::newRow("zero") << QByteArray::fromHex("00000000") << WeeChatProtocol::Integer(0);
        QTest::newRow("one") << QByteArray::fromHex("00000001") << WeeChatProtocol::Integer(1);
        QTest::newRow("minusone") << QByteArray::fromHex("FFFFFFFF") << WeeChatProtocol::Integer(-1);
    }
    void integer1() {
        QFETCH(QByteArray, input);
        QFETCH(WeeChatProtocol::Integer, output);

        QDataStream ds(&input, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::Integer>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
private slots:
    void long1_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<WeeChatProtocol::LongInteger>("output");
        QTest::newRow("basic") << (QByteArray::fromHex("03") + QByteArray("123")) << WeeChatProtocol::LongInteger(123);
        QTest::newRow("maximum") << (QByteArray::fromHex("13") + QByteArray("9223372036854775807"))
                                 << WeeChatProtocol::LongInteger(9223372036854775807LL);
        QTest::newRow("minimum") << (QByteArray::fromHex("14") + QByteArray("-9223372036854775808"))
                                 << WeeChatProtocol::LongInteger(-(9223372036854775807LL) - 1);
    }
    void long1() {
        QFETCH(QByteArray, input);
        QFETCH(WeeChatProtocol::LongInteger, output);

        QDataStream ds(&input, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::LongInteger>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
private slots:
    void string1_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<WeeChatProtocol::String>("output");
        QTest::newRow("null") << QByteArray::fromHex("FFFFFFFF") << WeeChatProtocol::String(QString {});
        QTest::newRow("empty") << QByteArray::fromHex("00000000") << WeeChatProtocol::String(QStringLiteral(""));
        QTest::newRow("helloworld") << (QByteArray::fromHex("0000000C") + QByteArray("Hello world!"))
                                    << WeeChatProtocol::String(QStringLiteral("Hello world!"));
    }
    void string1() {
        QFETCH(QByteArray, input);
        QFETCH(WeeChatProtocol::String, output);

        QDataStream ds(&input, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::String>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
private slots:
    void buffer1_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<WeeChatProtocol::Buffer>("output");
        // This is actually wrong in the docs it seems
        // QTest::newRow("null") << QByteArray::fromHex("FFFFFFFF") << WeeChatProtocol::Buffer({});
        QTest::newRow("empty") << QByteArray::fromHex("00000000") << WeeChatProtocol::Buffer("");
        QTest::newRow("helloworld") << (QByteArray::fromHex("00000004"
                                                            "0102FEFF"))
                                    << WeeChatProtocol::Buffer("\x01\x02\xFE\xFF");
    }
    void buffer1() {
        QFETCH(QByteArray, input);
        QFETCH(WeeChatProtocol::Buffer, output);

        QDataStream ds(&input, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::Buffer>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
private slots:
    void pointer1_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<WeeChatProtocol::Pointer>("output");
        QTest::newRow("docs_null") << QByteArray::fromHex(
                                          "01"
                                          "30"
                                      )
                                   << WeeChatProtocol::Pointer(0x0);
        QTest::newRow("docs_example") << QByteArray::fromHex(
                                             "09"
                                             "316132623363346435"
                                         )
                                      << WeeChatProtocol::Pointer(0x1a2b3c4d5);
    }
    void pointer1() {
        QFETCH(QByteArray, input);
        QFETCH(WeeChatProtocol::Pointer, output);

        QDataStream ds(&input, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::Pointer>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
private slots:
    void time1_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<WeeChatProtocol::Time>("output");
        QTest::newRow("docs_example") << QByteArray::fromHex(
                                             "0A"
                                             "31333231393933343536"
                                         )
                                      << WeeChatProtocol::Time("1321993456");
    }
    void time1() {
        QFETCH(QByteArray, input);
        QFETCH(WeeChatProtocol::Time, output);

        QDataStream ds(&input, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::Time>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
private slots:
    void array_str_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<WeeChatProtocol::ArrayStr>("output");
        QTest::newRow("empty") << QByteArray::fromHex("00000000") << WeeChatProtocol::ArrayStr({});
        QTest::newRow("docs_example") << QByteArray::fromHex(
                                             "00000002"
                                             "00000003"
                                             "616263"
                                             "00000002"
                                             "6465"
                                         )
                                      << WeeChatProtocol::ArrayStr({"abc", "de"});
    }
    void array_str() {
        QFETCH(QByteArray, input);
        QFETCH(WeeChatProtocol::ArrayStr, output);

        QDataStream ds(&input, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::ArrayStr>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
private slots:
    void array_int_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<WeeChatProtocol::ArrayInt>("output");
        QTest::newRow("empty") << QByteArray::fromHex("00000000") << WeeChatProtocol::ArrayInt({});
        QTest::newRow("docs_example") << QByteArray::fromHex(
                                             "00000003"
                                             "0000007B"
                                             "000001C8"
                                             "00000315"
                                         )
                                      << WeeChatProtocol::ArrayInt({0x7B, 0x1C8, 0x315});
    }
    void array_int() {
        QFETCH(QByteArray, input);
        QFETCH(WeeChatProtocol::ArrayInt, output);

        QDataStream ds(&input, QIODevice::ReadOnly);
        bool ok = false;
        auto result = WeeChatProtocol::parse<WeeChatProtocol::ArrayInt>(ds, &ok);
        QVERIFY(ok);
        QCOMPARE(result, output);
    }
};

QTEST_MAIN(ProtocolTest)
#include "ProtocolTest.moc"
