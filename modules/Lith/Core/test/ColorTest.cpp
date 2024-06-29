#include <QtTest/QtTest>

#include "formattedstring.h"
#include "protocol.h"

class ColorTest : public QObject {
    Q_OBJECT
private slots:
    void convertColors_data() {
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<QString>("htmlOutput");
        QTest::newRow("empty") << QByteArray("")
                               << QStringLiteral(
                                      "<span style='white-space: pre-wrap;'>"
                                      "</span>"
                                  );
#if 1
        QTest::newRow("time_open") << QByteArray(
                                          "\x19\x30\x32"
                                          "12"
                                          "\x19\x30\x33"
                                          ":"
                                          "\x19\x30\x32"
                                          "34"
                                          "\x19\x30\x33"
                                          ":"
                                          "\x19\x30\x32"
                                          "56"
                                      )
                                   << QStringLiteral(
                                          "<span style='white-space: pre-wrap;'>"
                                          "<font color=\"#444444\">12</font>"
                                          "<font color=\"#880000\">:</font>"
                                          "<font color=\"#444444\">34</font>"
                                          "<font color=\"#880000\">:</font>"
                                          "<font color=\"#444444\">56</font>"
                                          "</span>"
                                      );
        QTest::newRow("time_closed") << QByteArray(
                                            "\x19\x30\x32"
                                            "12"
                                            "\x19\x30\x33"
                                            ":"
                                            "\x19\x30\x32"
                                            "34"
                                            "\x19\x30\x33"
                                            ":"
                                            "\x19\x30\x32"
                                            "56"
                                            "\x1c"
                                        )
                                     << QStringLiteral(
                                            "<span style='white-space: pre-wrap;'>"
                                            "<font color=\"#444444\">12</font>"
                                            "<font color=\"#880000\">:</font>"
                                            "<font color=\"#444444\">34</font>"
                                            "<font color=\"#880000\">:</font>"
                                            "<font color=\"#444444\">56</font>"
                                            "</span>"
                                        );
        QTest::newRow("bold_closed") << QByteArray(
                                            "\x1a\x01"
                                            "bold text"
                                            "\x1b\x01"
                                        )
                                     << QStringLiteral(
                                            "<span style='white-space: pre-wrap;'>"
                                            "<b>bold text</b>"
                                            "</span>"
                                        );
        QTest::newRow("bold_open") << QByteArray(
                                          "\x1a\x01"
                                          "bold text"
                                      )
                                   << QStringLiteral(
                                          "<span style='white-space: pre-wrap;'>"
                                          "<b>bold text</b>"
                                          "</span>"
                                      );
        QTest::newRow("italic_closed") << QByteArray(
                                              "\x1a\x03"
                                              "italic text"
                                              "\x1b\x03"
                                          )
                                       << QStringLiteral(
                                              "<span style='white-space: pre-wrap;'>"
                                              "<i>italic text</i>"
                                              "</span>"
                                          );
        QTest::newRow("italic_open") << QByteArray(
                                            "\x1a\x03"
                                            "italic text"
                                        )
                                     << QStringLiteral(
                                            "<span style='white-space: pre-wrap;'>"
                                            "<i>italic text</i>"
                                            "</span>"
                                        );
        QTest::newRow("underline_closed") << QByteArray(
                                                 "\x1a\x04"
                                                 "underline text"
                                                 "\x1b\x04"
                                             )
                                          << QStringLiteral(
                                                 "<span style='white-space: pre-wrap;'>"
                                                 "<u>underline text</u>"
                                                 "</span>"
                                             );
        QTest::newRow("underline_open") << QByteArray(
                                               "\x1a\x04"
                                               "underline text"
                                           )
                                        << QStringLiteral(
                                               "<span style='white-space: pre-wrap;'>"
                                               "<u>underline text</u>"
                                               "</span>"
                                           );

        QTest::newRow("color then bold") << QByteArray(
                                                "\x19\x46\x30\x30"
                                                "colored"
                                                "\x1c"
                                                "regular 1"
                                                "\x1a\x01"
                                                "bold"
                                                "\x1b\x01"
                                                "regular 2"
                                            )
                                         << QStringLiteral(
                                                "<span style='white-space: pre-wrap;'>"
                                                "<font color=\"#000000\">colored</font>"
                                                "regular 1"
                                                "<b>bold</b>"
                                                "regular 2"
                                                "</span>"
                                            );

        QTest::newRow("ahoj00") << QByteArray(
                                       "\x1a\x01"
                                       "ah"
                                       "\x19\x46\x7c\x30\x39"
                                       "o"
                                       "\x19\x46\x7c\x30\x32"
                                       "jsefkoesafj"
                                       "\x1a\x02"
                                       "iergjherg"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<b>ah</b>"
                                       "<b><font color=\"#000088\">o</font></b>"
                                       "<b><font color=\"#444444\">jsefkoesafj</font></b>"
                                       "<b><span style='background: #444444'><font color=\"#ffffff\">iergjherg</font></span></b>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj01")
            << QByteArray(
                   "\x1a\x01"
                   "ah"
                   "\x19\x46\x7c\x30\x39"
                   "o"
                   "\x19\x46\x7c\x30\x32"
                   "jsefkoesafjiergjherg"
               )
            << QStringLiteral(
                   "<span style='white-space: pre-wrap;'>"
                   "<b>ah</b><b><font color=\"#000088\">o</font></b><b><font color=\"#444444\">jsefkoesafjiergjherg</font></b>"
                   "</span>"
               );
        QTest::newRow("ahoj02") << QByteArray(
                                       "\x1a\x01"
                                       "ah"
                                       "\x19\x46\x7c\x30\x39"
                                       "oj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<b>ah</b><b><font color=\"#000088\">oj</font></b>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj03") << QByteArray(
                                       "\x1a\x01"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<b>ahoj</b>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj04") << QByteArray(
                                       "\x19\x2a\x7c\x30\x37\x7e\x30\x37"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<span style='background: #d2691e'><font color=\"#d2691e\">ahoj</font></span>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj05") << QByteArray(
                                       "\x19\x2a\x7c\x30\x38\x7e\x30\x38"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<span style='background: #dddd00'><font color=\"#dddd00\">ahoj</font></span>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj06") << QByteArray(
                                       "\x19\x2a\x7c\x30\x36\x7e\x30\x36"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<span style='background: #33cc33'><font color=\"#33cc33\">ahoj</font></span>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj07") << QByteArray(
                                       "\x19\x46\x7c\x30\x36"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#33cc33\">ahoj</font>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj08") << QByteArray(
                                       "\x19\x46\x7c\x30\x38"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#dddd00\">ahoj</font>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj09") << QByteArray(
                                       "\x19\x46\x7c\x30\x37"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#d2691e\">ahoj</font>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj10") << QByteArray(
                                       "\x19\x46\x7c\x31\x31"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#660066\">ahoj</font>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj11") << QByteArray(
                                       "\x19\x46\x7c\x30\x32"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#444444\">ahoj</font>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj12") << QByteArray(
                                       "\x19\x46\x7c\x30\x39"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#000088\">ahoj</font>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj13") << QByteArray(
                                       "\x19\x46\x7c\x30\x35"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#008800\">ahoj</font>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj14") << QByteArray(
                                       "\x19\x46\x7c\x30\x34"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#ff4444\">ahoj</font>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj15") << QByteArray(
                                       "\x19\x2a\x7c\x30\x34\x7e\x30\x34"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<span style='background: #ff4444'><font color=\"#ff4444\">ahoj</font></span>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj16") << QByteArray(
                                       "\x19\x2a\x7c\x30\x35\x7e\x30\x35"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<span style='background: #008800'><font color=\"#008800\">ahoj</font></span>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj17") << QByteArray(
                                       "\x19\x2a\x7c\x30\x39\x7e\x30\x39"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<span style='background: #000088'><font color=\"#000088\">ahoj</font></span>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj18") << QByteArray(
                                       "\x19\x2a\x7c\x30\x31\x7e\x30\x31"
                                       "ahoj"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<span style='background: #ffffff'><font color=\"#ffffff\">ahoj</font></span>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj19") << QByteArray(
                                       "\x1a\x01"
                                       "AHOJ"
                                       "\x1c\x20\x1a\x03"
                                       "AHOJ"
                                       "\x1c\x20\x1a\x04"
                                       "AHOJ"
                                       "\x1c\x20\x1a\x02"
                                       "AHOJ"
                                       "\x1c"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<b>AHOJ</b> "
                                       "<i>AHOJ</i> "
                                       "<u>AHOJ</u> "
                                       "<span style='background: #000000'><font color=\"#ffffff\">AHOJ</font></span>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj20") << QByteArray(
                                       "\x19\x2a\x7c\x30\x33\x7e\x30\x35"
                                       "AHOJ"
                                       "\x1a\x01"
                                       "AHOJ"
                                       "\x1a\x02"
                                       "AHOJ"
                                       "\x1a\x03"
                                       "AHOJ"
                                       "\x19\x46\x7c\x30\x38"
                                       "AHOJ"
                                       ""
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<span style='background: #008800'><font color=\"#880000\">AHOJ</font></span>"
                                       "<b><span style='background: #008800'><font color=\"#880000\">AHOJ</font></span></b>"
                                       "<b><span style='background: #880000'><font color=\"#008800\">AHOJ</font></span></b>"
                                       "<i><b><span style='background: #880000'><font color=\"#008800\">AHOJ</font></span></b></i>"
                                       "<i><b><span style='background: #dddd00'><font color=\"#008800\">AHOJ</font></span></b></i>"
                                       "</span>"
                                   );
        QTest::newRow("ahoj21") << QByteArray(
                                       "\x19\x46\x7c\x30\x35"
                                       "AHOJ"
                                       "\x1a\x02"
                                       "AHOJ"
                                       "\x19\x46\x7c\x31\x31"
                                       "AHOJ"
                                       "\x1b\x02\x19\x42\x31\x33"
                                       "AHOJ"
                                       "\x1a\x02"
                                       "AHOJ"
                                       "\x19\x46\x7c\x30\x34"
                                       "AHOJ"
                                       "\x19\x42\x30\x38"
                                       "AHOJ"
                                       "\x1b\x02"
                                       "AHOJ"
                                       "\x1a\x02"
                                       "AHOJ"
                                   )
                                << QStringLiteral(
                                       "<span style='white-space: pre-wrap;'>"
                                       "<font color=\"#008800\">AHOJ</font>"
                                       "<span style='background: #008800'><font color=\"#ffffff\">AHOJ</font></span>"
                                       "<span style='background: #660066'><font color=\"#ffffff\">AHOJ</font></span>"
                                       "<span style='background: #006666'><font color=\"#660066\">AHOJ</font></span>"
                                       "<span style='background: #660066'><font color=\"#006666\">AHOJ</font></span>"
                                       "<span style='background: #ff4444'><font color=\"#006666\">AHOJ</font></span>"
                                       "<span style='background: #ff4444'><font color=\"#dddd00\">AHOJ</font></span>"
                                       "<span style='background: #dddd00'><font color=\"#ff4444\">AHOJ</font></span>"
                                       "<span style='background: #ff4444'><font color=\"#dddd00\">AHOJ</font></span>"
                                       "</span>"
                                   );


#endif
    }
    void convertColors() {
        QFETCH(QByteArray, input);
        QFETCH(QString, htmlOutput);

        auto result = WeeChatProtocol::convertColorsToHtml(input);
        QCOMPARE(result.toHtml(*lightTheme), htmlOutput);
    }
};

QTEST_MAIN(ColorTest)
#include "ColorTest.moc"
