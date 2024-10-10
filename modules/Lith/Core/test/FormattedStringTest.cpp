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

#include "formattedstring.h"
#include "protocol.h"

#include <QTextDocument>

class FormattedStringTest : public QObject {
    Q_OBJECT
private slots:
    void shortenUrls_data() {
        QTest::addColumn<int>("threshold");
        QTest::addColumn<QString>("input");
        QTest::addColumn<QString>("result");

        // TODO stop hardcoding string sizes but use sizeof or something like that
        // clang-format off

        // Rudimentary cases
        QTest::newRow("a")
            << 1
            << QStringLiteral("https://a")
            << QStringLiteral("https://a");
        QTest::newRow("a slash")
            << 11
            << QStringLiteral("https://a/")
            << QStringLiteral("https://a/");
        QTest::newRow("a.b.c")
            << 10
            << QStringLiteral("https://a.b.c")
            << QStringLiteral("https://a.b.c");

        // Basic URL
        QTest::newRow("example.com KEEP")
            << 20
            << QStringLiteral("https://example.com/")
            << QStringLiteral("https://example.com/");
        QTest::newRow("example.com TRIM_0_A")
            << 19
            << QStringLiteral("https://example.com/")
            << QStringLiteral("https://example.com/");
        QTest::newRow("example.com TRIM_0_B")
            << 1
            << QStringLiteral("https://example.com/")
            << QStringLiteral("https://example.com/");

        // File, query
        QTest::newRow("example.com path KEEP")
            << 30
            << QStringLiteral("https://example.com/12345.jpeg")
            << QStringLiteral("https://example.com/12345.jpeg");
        QTest::newRow("example.com path TRIM_0_A")
            << 29
            << QStringLiteral("https://example.com/12345.jpeg")
            << QStringLiteral("https://example.com/12345.jpeg");
        QTest::newRow("example.com path TRIM_0_B")
            << 1
            << QStringLiteral("https://example.com/12345.jpeg")
            << QStringLiteral("https://example.com/12345.jpeg");

        // File, query
        QTest::newRow("example.com path query KEEP")
            << 45
            << QStringLiteral("https://example.com/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com/12345.jpeg?somestuff=abcd");
        QTest::newRow("example.com path query TRIM_1_A")
            << 44
            << QStringLiteral("https://example.com/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com/12345.jpeg?\u2026");
        QTest::newRow("example.com path query TRIM_1_B")
            << 1
            << QStringLiteral("https://example.com/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com/12345.jpeg?\u2026");

        // Folder, file, query
        QTest::newRow("example.com folder path query KEEP")
            << 57
            << QStringLiteral("https://example.com/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com/some/folder/12345.jpeg?somestuff=abcd");
        QTest::newRow("example.com folder path query TRIM_1_A")
            << 56
            << QStringLiteral("https://example.com/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com folder path query TRIM_1_B")
            << 44
            << QStringLiteral("https://example.com/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com folder path query TRIM_2_A")
            << 43
            << QStringLiteral("https://example.com/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com/\u2026/12345.jpeg?\u2026");
        QTest::newRow("example.com folder path query TRIM_2_B")
            << 1
            << QStringLiteral("https://example.com/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com/\u2026/12345.jpeg?\u2026");

        // Port, folder, file, query
        QTest::newRow("example.com port folder path query KEEP")
            << 61
            << QStringLiteral("https://example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com:443/some/folder/12345.jpeg?somestuff=abcd");
        QTest::newRow("example.com port folder path query TRIM_1_A")
            << 60
            << QStringLiteral("https://example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com port folder path query TRIM_1_B")
            << 48
            << QStringLiteral("https://example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com port folder path query TRIM_2_A")
            << 47
            << QStringLiteral("https://example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com:443/\u2026/12345.jpeg?\u2026");
        QTest::newRow("example.com port folder path query TRIM_2_B")
            << 1
            << QStringLiteral("https://example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://example.com:443/\u2026/12345.jpeg?\u2026");

        // User, port, folder, file, query
        QTest::newRow("example.com user port folder path query KEEP")
            << 66
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?somestuff=abcd");
        QTest::newRow("example.com user port folder path query TRIM_1_A")
            << 65
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com user port folder path query TRIM_1_B")
            << 53
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com user port folder path query TRIM_2_A")
            << 52
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://\u2026@example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com user port folder path query TRIM_2_B")
            << 50
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://\u2026@example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com user port folder path query TRIM_3_A")
            << 49
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://\u2026@example.com:443/\u2026/12345.jpeg?\u2026");
        QTest::newRow("example.com user port folder path query TRIM_3_B")
            << 1
            << QStringLiteral("https://user@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://\u2026@example.com:443/\u2026/12345.jpeg?\u2026");

        // Everything
        QTest::newRow("example.com user password port folder path query KEEP")
            << 75
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?somestuff=abcd");
        QTest::newRow("example.com user password port folder path query TRIM_1_A")
            << 74
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com user password port folder path query TRIM_1_B")
            << 62
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com user password port folder path query TRIM_2_A")
            << 61
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://\u2026@example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com user password port folder path query TRIM_2_B")
            << 50
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://\u2026@example.com:443/some/folder/12345.jpeg?\u2026");
        QTest::newRow("example.com user password port folder path query TRIM_3_A")
            << 48
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://\u2026@example.com:443/\u2026/12345.jpeg?\u2026");
        QTest::newRow("example.com user password port folder path query TRIM_3_B")
            << 1
            << QStringLiteral("https://user:password@example.com:443/some/folder/12345.jpeg?somestuff=abcd")
            << QStringLiteral("https://\u2026@example.com:443/\u2026/12345.jpeg?\u2026");

        // No file
        QTest::newRow("example.com user password port folder KEEP")
            << 50
            << QStringLiteral("https://user:password@example.com:443/some/folder/")
            << QStringLiteral("https://user:password@example.com:443/some/folder/");
        QTest::newRow("example.com user password port folder TRIM_1")
            << 49
            << QStringLiteral("https://user:password@example.com:443/some/folder/")
            << QStringLiteral("https://user:password@example.com:443/some/folder");
        QTest::newRow("example.com user password port folder TRIM_2_A")
            << 48
            << QStringLiteral("https://user:password@example.com:443/some/folder/")
            << QStringLiteral("https://\u2026@example.com:443/some/folder");
        QTest::newRow("example.com user password port folder TRIM_2_B")
            << 37
            << QStringLiteral("https://user:password@example.com:443/some/folder/")
            << QStringLiteral("https://\u2026@example.com:443/some/folder");
        QTest::newRow("example.com user password port folder TRIM_3_B")
            << 36
            << QStringLiteral("https://user:password@example.com:443/some/folder/")
            << QStringLiteral("https://\u2026@example.com:443/\u2026/folder");
        QTest::newRow("example.com user password port folder TRIM_3_B")
            << 1
            << QStringLiteral("https://user:password@example.com:443/some/folder/")
            << QStringLiteral("https://\u2026@example.com:443/\u2026/folder");

        // clang-format on
    }
    void shortenUrls() {
        QFETCH(int, threshold);
        QFETCH(QString, input);
        QFETCH(QString, result);

        FormattedString fs(std::move(input));
        fs.prune();
        // Not the most efficient way to go about this but it's easier to just extract plaintext from HTML
        QTextDocument td;
        td.setHtml(fs.toHtml(*lightTheme, threshold));
        QCOMPARE(td.toPlainText(), result);
    }
};

QTEST_MAIN(FormattedStringTest)
#include "FormattedStringTest.moc"
