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
#include <QTimeZone>
#include <QAbstractEventDispatcher>
#include <QStringLiteral>
#include <cstring>
#include <functional>

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
            r = PlainString {QString("")};
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
        std::array<char, 3> keyTypeData {0, 0, 0};
        s.readRawData(keyTypeData.data(), 3);
        QByteArrayView keyType(keyTypeData.data(), 3);
        if (keyType != QByteArrayLiteral("str")) {
            qWarning() << "Hashtable currently supports only string keys";
            if (ok) {
                *ok = false;
            }
            return r;
        }
        std::array<char, 3> valueTypeData {0, 0, 0};
        s.readRawData(valueTypeData.data(), 3);
        QByteArrayView valueType(valueTypeData.data(), 3);
        if (valueType != QByteArrayLiteral("str")) {
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
                const auto& name = keys.first();
                const auto& type = keys.last();
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
                    std::array<char, 3> fieldTypeData {0, 0, 0};
                    s.readRawData(fieldTypeData.data(), 3);
                    QByteArrayView fieldType(fieldTypeData.data(), 3);
                    if (fieldType == QByteArrayLiteral("int")) {
                        ArrayInt a = parse<ArrayInt>(s, &innerOk);
                        if (!innerOk) {
                            if (outerOk) {
                                *outerOk = false;
                            }
                            return r;
                        }
                        item.objects[name] = QVariant::fromValue(a);
                    } else if (fieldType == QByteArrayLiteral("str")) {
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

    namespace Api {
        FormattedString stringFromJson(const QString& s) {
            if (s.isEmpty()) {
                return {};
            }
            return convertColorsToHtml(s.toUtf8());
        }

        QDateTime dateFromJson(const QString& s) {
            // Dates come with up to microsecond precision, QDateTime only parses milliseconds
            QString trimmed = s;
            auto dot = trimmed.indexOf(QLatin1Char('.'));
            if (dot >= 0) {
                auto end = dot + 1;
                int digits = 0;
                while (end < trimmed.size() && trimmed.at(end).isDigit() && digits < 3) {
                    ++end;
                    ++digits;
                }
                trimmed = trimmed.left(end) + QStringLiteral("Z");
            }
            auto dt = QDateTime::fromString(trimmed, Qt::ISODateWithMs);
            dt.setTimeZone(QTimeZone::UTC);
            return dt.toLocalTime();
        }

        namespace {
            StringMap stringMapFromJson(const QJsonObject& obj) {
                StringMap r;
                for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
                    r[it.key()] = it.value().toString();
                }
                return r;
            }

            QStringList stringListFromJson(const QJsonArray& arr) {
                QStringList r;
                r.reserve(arr.size());
                for (const auto& v : arr) {
                    r.append(v.toString());
                }
                return r;
            }

            weechat_pointer_t ptrFromJson(const QJsonValue& v) {
                return static_cast<weechat_pointer_t>(v.toInteger());
            }

            HData::Item bufferItem(const QJsonObject& buffer) {
                HData::Item item;
                auto localVariables = stringMapFromJson(buffer.value(QStringLiteral("local_variables")).toObject());
                // The API "name" is the full name ("irc.libera.#chan"), the binary protocol's is the local one ("libera.#chan")
                auto name = localVariables.value(QStringLiteral("name"), buffer.value(QStringLiteral("name")).toString());
                item.pointers = {ptrFromJson(buffer.value(QStringLiteral("id")))};
                item.objects[QStringLiteral("number")] = buffer.value(QStringLiteral("number")).toInt();
                item.objects[QStringLiteral("name")] = QVariant::fromValue(stringFromJson(name));
                item.objects[QStringLiteral("short_name")] =
                    QVariant::fromValue(stringFromJson(buffer.value(QStringLiteral("short_name")).toString()));
                item.objects[QStringLiteral("title")] =
                    QVariant::fromValue(stringFromJson(buffer.value(QStringLiteral("title")).toString()));
                item.objects[QStringLiteral("hidden")] = buffer.value(QStringLiteral("hidden")).toBool();
                item.objects[QStringLiteral("local_variables")] = QVariant::fromValue(localVariables);
                return item;
            }

            HData::Item lineItem(weechat_pointer_t bufferPtr, const QJsonObject& line, bool withBufferKey) {
                HData::Item item;
                auto linePtr = ptrFromJson(line.value(QStringLiteral("id")));
                if (withBufferKey) {
                    item.pointers = {linePtr};
                    item.objects[QStringLiteral("buffer")] = QVariant::fromValue(bufferPtr);
                } else {
                    item.pointers = {bufferPtr, linePtr};
                }
                item.objects[QStringLiteral("date")] = QVariant::fromValue(dateFromJson(line.value(QStringLiteral("date")).toString()));
                item.objects[QStringLiteral("displayed")] = line.value(QStringLiteral("displayed")).toBool();
                item.objects[QStringLiteral("highlight")] = line.value(QStringLiteral("highlight")).toBool();
                item.objects[QStringLiteral("prefix")] =
                    QVariant::fromValue(stringFromJson(line.value(QStringLiteral("prefix")).toString()));
                item.objects[QStringLiteral("message")] =
                    QVariant::fromValue(stringFromJson(line.value(QStringLiteral("message")).toString()));
                item.objects[QStringLiteral("tags_array")] = stringListFromJson(line.value(QStringLiteral("tags")).toArray());
                return item;
            }

            HData::Item nickOrGroupItem(weechat_pointer_t bufferPtr, const QJsonObject& obj, bool isGroup, int level) {
                HData::Item item;
                item.pointers = {bufferPtr, ptrFromJson(obj.value(QStringLiteral("id")))};
                item.objects[QStringLiteral("group")] = QVariant::fromValue<char>(isGroup ? 1 : 0);
                item.objects[QStringLiteral("visible")] =
                    QVariant::fromValue<char>(obj.value(QStringLiteral("visible")).toBool() ? 1 : 0);
                item.objects[QStringLiteral("level")] = level;
                item.objects[QStringLiteral("name")] = QVariant::fromValue(stringFromJson(obj.value(QStringLiteral("name")).toString()));
                item.objects[QStringLiteral("color")] = QVariant::fromValue(stringFromJson(obj.value(QStringLiteral("color")).toString()));
                item.objects[QStringLiteral("prefix")] =
                    QVariant::fromValue(stringFromJson(obj.value(QStringLiteral("prefix")).toString()));
                item.objects[QStringLiteral("prefix_color")] =
                    QVariant::fromValue(stringFromJson(obj.value(QStringLiteral("prefix_color")).toString()));
                return item;
            }
        }  // namespace

        HData bufferHData(const QJsonObject& buffer) {
            HData r;
            r.data.append(bufferItem(buffer));
            return r;
        }

        HData bufferHData(const QJsonArray& buffers) {
            HData r;
            for (const auto& v : buffers) {
                r.data.append(bufferItem(v.toObject()));
            }
            return r;
        }

        HData lineHData(weechat_pointer_t bufferPtr, const QJsonObject& line, bool withBufferKey) {
            HData r;
            r.data.append(lineItem(bufferPtr, line, withBufferKey));
            return r;
        }

        HData lineHData(weechat_pointer_t bufferPtr, const QJsonArray& lines, bool withBufferKey) {
            HData r;
            // The API sends lines oldest first, the binary protocol's last_line(-N) newest first
            for (auto i = lines.size() - 1; i >= 0; i--) {
                r.data.append(lineItem(bufferPtr, lines.at(i).toObject(), withBufferKey));
            }
            return r;
        }

        HData nickHData(weechat_pointer_t bufferPtr, const QJsonObject& nicklistRoot) {
            HData r;
            std::function<void(const QJsonObject&, int)> walk = [&](const QJsonObject& group, int level) {
                const auto groups = group.value(QStringLiteral("groups")).toArray();
                for (const auto& gv : groups) {
                    auto g = gv.toObject();
                    r.data.append(nickOrGroupItem(bufferPtr, g, true, level));
                    walk(g, level + 1);
                }
                const auto nicks = group.value(QStringLiteral("nicks")).toArray();
                for (const auto& nv : nicks) {
                    // Nicks are always level 0 in the binary protocol, only groups carry their depth
                    r.data.append(nickOrGroupItem(bufferPtr, nv.toObject(), false, 0));
                }
            };
            walk(nicklistRoot, 1);
            return r;
        }

        HData nickDiffHData(weechat_pointer_t bufferPtr, const QJsonObject& nickOrGroup, bool isGroup, char op) {
            HData r;
            auto item = nickOrGroupItem(bufferPtr, nickOrGroup, isGroup, isGroup ? 1 : 0);
            item.objects[QStringLiteral("_diff")] = QVariant::fromValue<char>(op);
            r.data.append(item);
            return r;
        }

        HData hotlistHData(const QJsonArray& hotlist) {
            HData r;
            for (const auto& hv : hotlist) {
                auto h = hv.toObject();
                HData::Item item;
                auto bufPtr = ptrFromJson(h.value(QStringLiteral("buffer_id")));
                // Hotlist entries have no id in the API, there's at most one per buffer
                item.pointers = {bufPtr};
                item.objects[QStringLiteral("buffer")] = QVariant::fromValue(bufPtr);
                QList<int> counts;
                for (const auto& c : h.value(QStringLiteral("count")).toArray()) {
                    counts.append(c.toInt());
                }
                item.objects[QStringLiteral("count")] = QVariant::fromValue(counts);
                r.data.append(item);
            }
            return r;
        }
    }  // namespace Api

}  // namespace WeeChatProtocol
