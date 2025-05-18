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

#include "protocol.h"

#include <QDataStream>
#include <QApplication>
#include <QDateTime>
#include <QAbstractEventDispatcher>
#include <QStringLiteral>
#include <cstring>

namespace {
    Q_GLOBAL_STATIC(QByteArray, parserBuffer)
}

namespace WeeChatProtocol {
    template <> Char parse(QDataStream& s, bool* ok) {
        Char r {};
        s.readRawData(&r, 1);
        if (ok) {
            *ok = true;
        }
        return r;
    }

    template <> Integer parse(QDataStream& s, bool* ok) {
        Integer r {};
        s.setByteOrder(QDataStream::BigEndian);
        s >> r;
        if (ok) {
            *ok = true;
        }
        return r;
    }

    template <> LongInteger parse(QDataStream& s, bool* ok) {
        LongInteger r {};
        quint8 length = 0;
        s >> length;
        QByteArray buf(static_cast<int>(length), 0);
        s.readRawData(buf.data(), length);
        r = buf.toLongLong();
        if (ok) {
            *ok = true;
        }
        return r;
    }

    template <> PlainString parse(QDataStream& s, bool* ok) {
        PlainString r {};
        uint32_t len = 0;
        s >> len;
        if (len == static_cast<uint32_t>(-1)) {
            r = PlainString {};
        } else if (len == 0) {
            r = PlainString {QLatin1String("")};
        } else if (len > 0) {
            parserBuffer->resize(len);
            s.readRawData(parserBuffer->data(), static_cast<int>(len));
            r = QString::fromUtf8(*parserBuffer);
        }
        if (ok) {
            *ok = true;
        }
        return r;
    }

    template <> String parse(QDataStream& s, bool* ok) {
        String r {};
        uint32_t len = 0;
        s >> len;
        if (len == static_cast<uint32_t>(-1)) {
            r = String();
        } else if (len == 0) {
            r.addChar(QByteArrayLiteral(""));
        } else if (len > 0) {
            parserBuffer->resize(len);
            s.readRawData(parserBuffer->data(), static_cast<int>(len));
            r = convertColorsToHtml(*parserBuffer);
        }
        if (ok) {
            *ok = true;
        }
        return r;
    }

    template <> Buffer parse(QDataStream& s, bool* ok) {
        Buffer r {};
        uint32_t len = 0;
        r.clear();
        s >> len;
        if (len == 0) {
            r = "";
        }
        if (len > 0) {
            r = QByteArray(len, 0);
            s.readRawData(r.data(), static_cast<int>(len));
        }
        if (ok) {
            *ok = true;
        }
        return r;
    }

    template <> Pointer parse(QDataStream& s, bool* ok) {
        Pointer r {};
        quint8 length = 0;
        s >> length;
        QByteArray buf(static_cast<int>(length), 0);
        s.readRawData(buf.data(), length);
        bool parseOk = false;
        r = buf.toULongLong(&parseOk, 16);
        if (ok) {
            *ok = parseOk;
        }
        return r;
    }

    Time parseTime(QDataStream& s, bool* ok) {
        Time r {};
        quint8 length = 0;
        s >> length;
        parserBuffer->resize(static_cast<int>(length), 0);
        s.readRawData(parserBuffer->data(), length);
        r = parserBuffer->toLongLong(ok);
        return r;
    }

    template <> HashTable parse(QDataStream& s, bool* ok) {
        HashTable r {};
        std::array<char, 4> keyType {};
        keyType.fill(0);
        std::array<char, 4> valueType {};
        valueType.fill(0);
        s.readRawData(keyType.data(), 3);
        if (strcmp(keyType.data(), "str") != 0) {
            qWarning() << "Hashtable currently supports only string keys";
            if (ok) {
                *ok = false;
            }
            return r;
        }
        s.readRawData(valueType.data(), 3);
        if (strcmp(valueType.data(), "str") != 0) {
            qWarning() << "Hashtable currently supports only string values";
            if (ok) {
                *ok = false;
            }
            return r;
        }
        quint32 count = 0;
        s >> count;
        r.clear();
        for (quint32 i = 0; i < count; i++) {
            auto key = parse<PlainString>(s);
            auto value = parse<String>(s);
            r.insert(key, value.toPlain());
        }
        if (ok) {
            *ok = true;
        }
        return r;
    }

    template <> HData parse(QDataStream& s, bool* outerOk) {
        HData r {};
        bool innerOk = false;
        PlainString hpath = parse<PlainString>(s, &innerOk);
        if (!innerOk) {
            if (outerOk) {
                *outerOk = false;
            }
            return r;
        }
        PlainString keys = parse<PlainString>(s, &innerOk);
        if (!innerOk) {
            if (outerOk) {
                *outerOk = false;
            }
            return r;
        }
        Integer count = parse<Integer>(s, &innerOk);
        if (!innerOk) {
            if (outerOk) {
                *outerOk = false;
            }
            return r;
        }
        r.path = hpath.split(QStringLiteral("/"));
        r.keys = keys.split(QStringLiteral(","));

        for (int i = 0; i < count; i++) {
            HData::Item item;
            for (int j = 0; j < r.path.count(); j++) {
                Pointer ptr = parse<Pointer>(s, &innerOk);
                if (!innerOk) {
                    if (outerOk) {
                        *outerOk = false;
                    }
                    return r;
                }
                item.pointers.append(ptr);
            }
            for (int j = 0; j < r.keys.count(); j++) {
                auto keys = r.keys.at(j).split(QStringLiteral(":"));
                auto name = keys.first();
                auto type = keys.last();
                if (type == QStringLiteral("int")) {
                    Integer i = parse<Integer>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk) {
                            *outerOk = false;
                        }
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(i);
                } else if (type == QStringLiteral("lon")) {
                    LongInteger l = parse<LongInteger>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk) {
                            *outerOk = false;
                        }
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(l);
                } else if (type == QStringLiteral("str") || type == QStringLiteral("buf")) {
                    /* TODO this may be useful for optimization
                    bool canContainHTML = false;
                    if (name == QStringLiteral("message") || name == QStringLiteral("title") || name == QStringLiteral("prefix")) {
                        canContainHTML = true;
                    }
                    */
                    String str = parse<String>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk) {
                            *outerOk = false;
                        }
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(str);
                } else if (type == QStringLiteral("arr")) {
                    std::array<char, 4> fieldType {};
                    fieldType.fill(0);
                    s.readRawData(fieldType.data(), 3);
                    if (strcmp(fieldType.data(), "int") == 0) {
                        ArrayInt a = parse<ArrayInt>(s, &innerOk);
                        if (!innerOk) {
                            if (outerOk) {
                                *outerOk = false;
                            }
                            return r;
                        }
                        item.objects[name] = QVariant::fromValue(a);
                    } else if (strcmp(fieldType.data(), "str") == 0) {
                        ArrayStr a = parse<ArrayStr>(s, &innerOk);
                        if (!innerOk) {
                            if (outerOk) {
                                *outerOk = false;
                            }
                            return r;
                        }
                        item.objects[name] = QVariant::fromValue(a);
                    } else {
                        qCritical() << "Unhandled array item type:" << fieldType.data() << "for field" << name;
                    }
                } else if (type == QStringLiteral("tim")) {
                    Time t = parse<Time>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk) {
                            *outerOk = false;
                        }
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(QDateTime::fromSecsSinceEpoch(t));
                } else if (type == QStringLiteral("ptr")) {
                    Pointer p = parse<Pointer>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk) {
                            *outerOk = false;
                        }
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(p);
                } else if (type == QStringLiteral("chr")) {
                    Char c = parse<Char>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk) {
                            *outerOk = false;
                        }
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(c);
                } else if (type == QStringLiteral("htb")) {
                    HashTable htb = parse<HashTable>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk) {
                            *outerOk = false;
                        }
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(htb);
                } else {
                    qCritical() << "!!! Unhandled type:" << type << "for field" << name;
                }
            }
            r.data.append(item);
        }
        if (outerOk) {
            *outerOk = true;
        }
        return r;
    }

    template <> ArrayInt parse(QDataStream& s, bool* outerOk) {
        ArrayInt r {};
        uint32_t len = 0;
        s >> len;
        for (uint32_t i = 0; i < len; i++) {
            bool innerOk = false;
            Integer num = parse<Integer>(s, &innerOk);
            if (!innerOk) {
                if (outerOk) {
                    *outerOk = false;
                }
                return r;
            }
            r.append(num);
        }
        if (outerOk) {
            *outerOk = true;
        }
        return r;
    }

    template <> ArrayStr parse(QDataStream& s, bool* outerOk) {
        ArrayStr r {};
        uint32_t len = 0;
        s >> len;
        for (uint32_t i = 0; i < len; i++) {
            bool innerOk = false;
            String str = parse<String>(s, &innerOk);
            if (!innerOk) {
                if (outerOk) {
                    *outerOk = false;
                }
                return r;
            }
            r.emplace_back(str.toPlain());
        }
        if (outerOk) {
            *outerOk = true;
        }
        return r;
    }

    FormattedString convertColorsToHtml(const QByteArray& data) {
        FormattedString result;

        FormattedString::Part::Color foregroundColor;
        bool foreground = false;
        FormattedString::Part::Color backgroundColor;
        bool background = false;
        bool bold = false;
        bool reverse = false;
        bool italic = false;
        bool underline = false;
        bool keep = false;

        auto carryOver = [&result, &foregroundColor, &foreground, &backgroundColor, &background, &bold, &reverse, &italic, &underline,
                          &keep]() {
            FormattedString::Part& part = result.addPart();

            if (foreground) {
                part.foreground = foregroundColor;
            }
            if (background) {
                part.background = backgroundColor;
            }
            if (bold) {
                part.bold = true;
            }
            if (reverse) {
                auto oldBackground = part.background;
                if (part.foreground.index < 0) {
                    part.background.index = 0;
                    part.background.extended = false;
                } else {
                    part.background = part.foreground;
                }
                if (oldBackground.index < 0) {
                    part.foreground.index = 1;
                    part.foreground.extended = false;
                } else {
                    part.foreground = oldBackground;
                }
            }
            if (italic) {
                part.italic = true;
            }
            if (underline) {
                part.underline = true;
            }
            // if (keep) TODO
        };
        auto endColors = [&carryOver, &foreground, &background]() {
            if (background) {
                background = false;
            }
            if (foreground) {
                foreground = false;
            }
            carryOver();
        };
        auto endAttrs = [&carryOver, &bold, &reverse, &italic, &underline, &keep]() {
            if (bold) {
                bold = false;
            }
            if (reverse) {
                reverse = false;
            }
            if (italic) {
                italic = false;
            }
            if (underline) {
                underline = false;
            }
            if (keep) {
                // TODO
            }
            carryOver();
        };
        auto loadAttr = [&carryOver, &bold, &reverse, &italic, &underline, &keep](QByteArray::const_iterator& it) {
            while (true) {
                switch (*it) {
                    case 0x01:  // fallthrough // TODO what the fuck weechat
                    case '*':
                        if (bold) {
                            break;
                        }
                        bold = true;
                        carryOver();
                        break;
                    case 0x02:  // fallthrough
                    case '!':
                        if (reverse) {
                            break;
                        }
                        reverse = true;
                        carryOver();
                        break;
                    case 0x03:  // fallthrough
                    case '/':
                        if (italic) {
                            break;
                        }
                        italic = true;
                        carryOver();
                        break;
                    case 0x04:  // fallthrough
                    case '_':
                        if (underline) {
                            break;
                        }
                        underline = true;
                        carryOver();
                        break;
                    case '|':  // fallthrough
                    case '@':  // fallthrough
                    case 0x1A:
                        break;
                    default:
                        --it;
                        return;
                }
                ++it;
            }
        };

        auto clearAttr = [&carryOver, &bold, &reverse, &italic, &underline, &keep](QByteArray::const_iterator& it) {
            while (true) {
                switch (*it) {
                    case 0x01:  // fallthrough // TODO what the fuck weechat
                    case '*':
                        if (bold) {
                            bold = false;
                            carryOver();
                        }
                        break;
                    case 0x02:  // fallthrough
                    case '!':
                        if (reverse) {
                            reverse = false;
                            carryOver();
                        }
                        break;
                    case 0x03:  // fallthrough
                    case '/':
                        if (italic) {
                            italic = false;
                            carryOver();
                        }
                        break;
                    case 0x04:  // fallthrough
                    case '_':
                        if (underline) {
                            underline = false;
                            carryOver();
                        }
                        break;
                    case '|':  // fallthrough
                    case 0x1B:
                        break;
                    default:
                        --it;
                        return;
                }
                ++it;
            }
        };
        auto loadStd = [&carryOver, &foreground, &foregroundColor](QByteArray::const_iterator& it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|') {
                ++it;
            }
            int code = 0;
            if (*it == 0x19 || *it == 'F') {
                it++;
            }
            for (int i = 0; i < 2; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (foreground) {
                foreground = false;
            }
            if (code == 0) {
                foreground = true;
                foregroundColor.extended = false;
                foregroundColor.index = ColorTheme::FOREGROUND;
            } else if (code > 0 && code < ColorTheme::_LAST_WEECHAT_COLOR) {
                foreground = true;
                foregroundColor.extended = false;
                foregroundColor.index = code;
            }
            carryOver();
        };
        auto loadExt = [&carryOver, &foreground, &foregroundColor](QByteArray::const_iterator& it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|') {
                ++it;
            }
            int code = 0;
            for (int i = 0; i < 5; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (foreground) {
                foreground = false;
            }
            if (code >= 0 && code < ColorTheme::ExtendedColorCount) {
                foreground = true;
                foregroundColor.extended = true;
                foregroundColor.index = code;
            }
            carryOver();
        };
        auto loadBgStd = [&carryOver, &background, &backgroundColor](QByteArray::const_iterator& it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|' || *it == ',' || *it == '~') {
                ++it;
            }
            int code = 0;
            for (int i = 0; i < 2; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (background) {
                background = false;
            }
            if (code == 0) {
                background = true;
                backgroundColor.extended = false;
                backgroundColor.index = ColorTheme::BACKGROUND;
            } else if (code >= 0 && code < ColorTheme::_LAST_WEECHAT_COLOR) {
                background = true;
                backgroundColor.extended = false;
                backgroundColor.index = code;
            }
            carryOver();
        };
        auto loadBgExt = [&carryOver, &background, &backgroundColor](QByteArray::const_iterator& it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|' || *it == ',' || *it == '~') {
                ++it;
            }
            int code = 0;
            for (int i = 0; i < 5; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (background) {
                background = false;
            }
            if (code >= 0 && code < ColorTheme::ExtendedColorCount) {
                background = true;
                backgroundColor.extended = true;
                backgroundColor.index = code;
            }
            carryOver();
        };
        auto getChar = [](QByteArray::const_iterator& it) -> const QByteArray& {
            static QByteArray buf;
            buf.resize(1);
            if (static_cast<unsigned char>(*it) < 0x80) {
                buf[0] = *it;
            } else {
                if ((*it & 0b11111000U) == 0b11110000U) {
                    buf.resize(4);
                    buf[0] = *it++;
                    buf[1] = *it++;
                    buf[2] = *it++;
                    buf[3] = *it;
                } else if ((*it & 0b11110000U) == 0b11100000U) {
                    buf.resize(3);
                    buf[0] = *it++;
                    buf[1] = *it++;
                    buf[2] = *it;
                } else if ((*it & 0b11100000U) == 0b11000000U) {
                    buf.resize(2);
                    buf[0] = *it++;
                    buf[1] = *it;
                } else {
                    buf[0] = *it;
                }
            }
            return buf;
        };
        for (const auto* it = data.begin(); it != data.end(); ++it) {
            if (*it == 0x19) {
                ++it;
                if (*it == 'F') {
                    ++it;
                    if (*it == '@') {
                        ++it;
                        loadAttr(it);
                        loadExt(it);
                    } else {
                        loadAttr(it);
                        loadStd(it);
                    }
                } else if (*it == 'B') {
                    ++it;
                    if (*it == '@') {
                        loadBgExt(it);
                    } else {
                        loadBgStd(it);
                    }
                } else if (*it == '*') {
                    ++it;
                    if (*it == '@') {
                        ++it;
                        loadAttr(it);
                        loadExt(it);
                    } else {
                        loadAttr(it);
                        loadStd(it);
                    }
                    ++it;
                    if (*it == ',' || *it == '~') {
                        ++it;
                        if (*it == '@') {
                            ++it;
                            loadAttr(it);
                            loadBgExt(it);
                        } else {
                            loadAttr(it);
                            loadBgStd(it);
                        }
                    } else {
                        --it;
                    }
                } else if (*it == '@') {
                    ++it;
                    loadExt(it);
                } else if (*it == 0x1C) {
                    endColors();
                } else {
                    loadStd(it);
                }
            } else if (*it == 0x1C) {
                endColors();
                endAttrs();
            } else if (*it == 0x1A) {
                loadAttr(it);
            } else if (*it == 0x1B) {
                clearAttr(it);
            } else if (*it) {
                result.addChar(getChar(it));
            }
        }
        endColors();
        endAttrs();

        result.prune();

        return result;
    }

    QString HData::toString() const {
        QString ret;

        ret += QStringLiteral("HDATA\n");
        ret += QStringLiteral("- PATH:\n");
        for (const auto& i : path) {
            ret += QStringLiteral("\t") + i + QStringLiteral("\n");
        }
        ret += QStringLiteral("- KEYS:\n");
        for (const auto& i : keys) {
            ret += QStringLiteral("\t") + i + QStringLiteral("\n");
        }
        ret += QStringLiteral("-VALUES:\n");
        for (const auto& i : data) {
            ret += QStringLiteral("\t-PATH\n");
            ret += QStringLiteral("\t\t");
            for (const auto& j : i.pointers) {
                auto hex = QString::number(j, 16);
                ret += hex + QStringLiteral(" ");
            }
            ret += QStringLiteral("\n");
            ret += QStringLiteral("\t-OBJECTS\n");
            for (auto [key, value] : i.objects.asKeyValueRange()) {
                ret += QStringLiteral("\t\t") + key + QStringLiteral(": \"") + value.toString() + QStringLiteral("\"\n");
            }
            ret += QStringLiteral("\n");
        }
        return ret;
    }

}  // namespace WeeChatProtocol
