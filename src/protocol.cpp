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

#include "lith.h"

#include <QDataStream>
#include <QApplication>
#include <QDateTime>
#include <QAbstractEventDispatcher>
#include <QStringLiteral>

namespace WeeChatProtocol {

template <>
Char parse(QDataStream &s, bool *ok) {
    Char r;
    s.readRawData(&r, 1);
    if (ok)
        *ok = true;
    return r;
}

template <>
Integer parse(QDataStream &s, bool *ok) {
    Integer r;
    s.setByteOrder(QDataStream::BigEndian);
    s >> r;
    if (ok)
        *ok = true;
    return r;
}

template <>
LongInteger parse(QDataStream &s, bool *ok) {
    LongInteger r;
    quint8 length;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    r = buf.toLongLong();
    if (ok)
        *ok = true;
    return r;
}

template <>
String parse(QDataStream &s, bool canContainHtml, bool *ok) {
    String r;
    uint32_t len;
    r.clear();
    s >> len;
    if (len == uint32_t(-1))
        r = String();
    else if (len == 0)
        r = "";
    else if (len > 0) {
        QByteArray buf(len + 1, 0);
        s.readRawData(buf.data(), len);
        r = convertColorsToHtml(buf, canContainHtml);
    }
    if (ok)
        *ok = true;
    return r;
}

template<>
String parse(QDataStream &s, bool *ok) {
    return parse<String>(s, false, ok);
}

template <>
Buffer parse(QDataStream &s, bool *ok) {
    Buffer r;
    uint32_t len;
    r.clear();
    s >> len;
    if (len == 0)
        r = "";
    if (len > 0) {
        r = QByteArray(len, 0);
        s.readRawData(r.data(), len);
    }
    if (ok)
        *ok = true;
    return r;
}

template <>
Pointer parse(QDataStream &s, bool *ok) {
    Pointer r;
    quint8 length;;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    bool parseOk = false;
    r = buf.toULongLong(&parseOk, 16);
    if (ok)
        *ok = parseOk;
    return r;
}

template <>
Time parse(QDataStream &s, bool *ok) {
    Time r;
    quint8 length;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    r = buf;
    if (ok)
        *ok = true;
    return r;
}

template <>
HashTable parse(QDataStream &s, bool *ok) {
    HashTable r;
    char keyType[4] = { 0 }, valueType[4] = { 0 };
    s.readRawData(keyType, 3);
    if (QString(keyType) != "str") {
        qWarning() << "Hashtable currently supports only string keys";
        if (ok)
            *ok = false;
        return r;
    }
    s.readRawData(valueType, 3);
    if (QString(valueType) != "str") {
        qWarning() << "Hashtable currently supports only string values";
        if (ok)
            *ok = false;
        return r;
    }
    quint32 count = 0;
    s >> count;
    r.clear();
    for (quint32 i = 0; i < count; i++) {
        auto key = parse<String>(s);
        auto value = parse<String>(s);
        r.insert(key, value);
    }
    if (ok)
        *ok = true;
    return r;
}

template <>
HData parse(QDataStream &s, bool *outerOk) {
    HData r;
    bool innerOk = false;
    String hpath = parse<String>(s, &innerOk);
    if (!innerOk) {
        if (outerOk)
            *outerOk = false;
        return r;
    }
    String keys = parse<String>(s, &innerOk);
    if (!innerOk) {
        if (outerOk)
            *outerOk = false;
        return r;
    }
    Integer count = parse<Integer>(s, &innerOk);
    if (!innerOk) {
        if (outerOk)
            *outerOk = false;
        return r;
    }
    r.path = hpath.split("/");
    r.keys = keys.split(",");

    for (int i = 0; i < count; i++) {
        HData::Item item;
        for (int j = 0; j < r.path.count(); j++) {
            Pointer ptr = parse<Pointer>(s, &innerOk);
            if (!innerOk) {
                if (outerOk)
                    *outerOk = false;
                return r;
            }
            item.pointers.append(ptr);
        }
        for (int j = 0; j < r.keys.count(); j++) {
            auto name = r.keys[j].split(":").first();
            auto type = r.keys[j].split(":").last();
            if (type == "int") {
                Integer i = parse<Integer>(s, &innerOk);
                if (!innerOk) {
                    if (outerOk)
                        *outerOk = false;
                    return r;
                }
                item.objects[name] = QVariant::fromValue(i);
            }
            else if (type == "lon") {
                LongInteger l = parse<LongInteger>(s, &innerOk);
                if (!innerOk) {
                    if (outerOk)
                        *outerOk = false;
                    return r;
                }
                item.objects[name] = QVariant::fromValue(l);
            }
            else if (type == "str" || type == "buf") {
                bool canContainHTML = false;
                if (name == "message" || name == "title" || name == "prefix")
                    canContainHTML = true;
                String str = parse<String>(s, canContainHTML, &innerOk);
                if (!innerOk) {
                    if (outerOk)
                        *outerOk = false;
                    return r;
                }
                item.objects[name] = QVariant::fromValue(str);
            }
            else if (type == "arr") {
                char fieldType[4] = { 0 };
                s.readRawData(fieldType, 3);
                if (strcmp(fieldType, "int") == 0) {
                    ArrayInt a = parse<ArrayInt>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk)
                            *outerOk = false;
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(a);
                }
                else if (strcmp(fieldType, "str") == 0) {
                    ArrayStr a = parse<ArrayStr>(s, &innerOk);
                    if (!innerOk) {
                        if (outerOk)
                            *outerOk = false;
                        return r;
                    }
                    item.objects[name] = QVariant::fromValue(a);
                }
                else {
                    qCritical() << "Unhandled array item type:" << fieldType << "for field" << name;
                }
            }
            else if (type == "tim") {
                Time t = parse<Time>(s, &innerOk);
                if (!innerOk) {
                    if (outerOk)
                        *outerOk = false;
                    return r;
                }
                item.objects[name] = QVariant::fromValue(QDateTime::fromSecsSinceEpoch(t.toLocal8Bit().toULongLong(nullptr, 10)));
            }
            else if (type == "ptr") {
                Pointer p = parse<Pointer>(s, &innerOk);
                if (!innerOk) {
                    if (outerOk)
                        *outerOk = false;
                    return r;
                }
                item.objects[name] = QVariant::fromValue(p);
            }
            else if (type == "chr") {
                Char c = parse<Char>(s, &innerOk);
                if (!innerOk) {
                    if (outerOk)
                        *outerOk = false;
                    return r;
                }
                item.objects[name] = QVariant::fromValue(c);
            }
            else if (type == "htb") {
                HashTable htb = parse<HashTable>(s, &innerOk);
                if (!innerOk) {
                    if (outerOk)
                        *outerOk = false;
                    return r;
                }
                item.objects[name] = QVariant::fromValue(htb);
            }
            else {
                qCritical() << "!!! Unhandled type:" << type << "for field" << name;
            }
        }
        r.data.append(item);
    }
    if (outerOk)
        *outerOk = true;
    return r;
}

template <>
ArrayInt parse(QDataStream &s, bool *outerOk) {
    ArrayInt r;
    uint32_t len;
    s >> len;
    for (uint32_t i = 0; i < len; i++) {
        bool innerOk = false;
        Integer num = parse<Integer>(s, &innerOk);
        if (!innerOk) {
            if (outerOk)
                *outerOk = false;
            return r;
        }
        r.append(num);
    }
    if (outerOk)
        *outerOk = true;
    return r;
}

template <>
ArrayStr parse(QDataStream &s, bool *outerOk) {
    ArrayStr r;
    uint32_t len;
    s >> len;
    for (uint32_t i = 0; i < len; i++) {
        bool innerOk = false;
        String str = parse<String>(s, &innerOk);
        if (!innerOk) {
            if (outerOk)
                *outerOk = false;
            return r;
        }
        r.append(str);
    }
    if (outerOk)
        *outerOk = true;
    return r;
}

FormattedString convertColorsToHtml(const QByteArray &data, bool canContainHtml) {
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

    auto carryOver = [&result, &foregroundColor, &foreground, &backgroundColor, &background, &bold, &reverse, &italic, &underline, &keep]() {
        auto &part = result.addPart();

        if (foreground)
            part.foreground = foregroundColor;
        if (background)
            part.background = backgroundColor;
        if (bold)
            part.bold = true;
        // if (reverse) TODO
        if (italic)
            part.italic = true;
        if (underline)
            part.underline = true;
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
           // TODO
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
    auto loadAttr = [&carryOver, &bold, &reverse, &italic, &underline, &keep](QByteArray::const_iterator &it) {
       while (true) {
           switch(*it) {
           case 0x01: // fallthrough // TODO what the fuck weechat
           case '*':
               if (bold)
                   break;
               bold = true;
               carryOver();
               break;
           case 0x02: // fallthrough
           case '!':
               if (reverse)
                   break;
               reverse = true;
               carryOver();
               break;
           case 0x03: // fallthrough
           case '/':
               if (italic)
                   break;
               italic = true;
               carryOver();
               break;
           case 0x04: // fallthrough
           case '_':
               if (underline)
                   break;
               underline = true;
               carryOver();
               break;
           case '|':
               break;
           case '@':
               break;
           case 0x1A:
               break;
           default:
               --it;
               return;
           }
           ++it;
       }
    };

    auto clearAttr = [&carryOver, &bold, &reverse, &italic, &underline, &keep](QByteArray::const_iterator &it) {
       while (true) {
           switch(*it) {
           case 0x01: // fallthrough // TODO what the fuck weechat
           case '*':
               if (bold) {
                   bold = false;
                   carryOver();
               }
               break;
           case 0x02: // fallthrough
           case '!':
               if (reverse) {
                   reverse = false;
                   carryOver();
               }
               break;
           case 0x03: // fallthrough
           case '/':
               if (italic) {
                   italic = false;
                   carryOver();
               }
               break;
           case 0x04: // fallthrough
           case '_':
               if (underline) {
                   underline = false;
                   carryOver();
               }
               break;
           case '|':
               break;
           case 0x1B:
               break;
           default:
               --it;
               return;
           }
           ++it;
       }
    };
    auto loadStd = [&carryOver, &foreground, &foregroundColor](QByteArray::const_iterator &it) {
       while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
           ++it;
       int code = 0;
       if (*it == 0x19 || *it == 'F')
           it++;
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
       }
       else if (code > 0 && code < ColorTheme::_LAST_WEECHAT_COLOR) {
           foreground = true;
           foregroundColor.extended = false;
           foregroundColor.index = code;
       }
       carryOver();
    };
    auto loadExt = [&carryOver, &foreground, &foregroundColor](QByteArray::const_iterator &it) {
        while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
           ++it;
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
    auto loadBgStd = [&carryOver, &background, &backgroundColor](QByteArray::const_iterator &it) {
       while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|' || *it == ',' || *it == '~')
           ++it;
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
       }
       else if (code >= 0 && code < ColorTheme::_LAST_WEECHAT_COLOR) {
           background = true;
           backgroundColor.extended = false;
           backgroundColor.index = code;
       }
       carryOver();
    };
    auto loadBgExt = [&carryOver, &background, &backgroundColor](QByteArray::const_iterator &it) {
       while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|' || *it == ',' || *it == '~')
           ++it;
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
    auto getChar = [](QByteArray::const_iterator &it) -> QString {
       if ((unsigned char) *it < 0x80) {
           return QString(*it);
       }
       else {
           QByteArray buf;
           if ((*it & 0b11111000) == 0b11110000) {
               buf += *it++;
               buf += *it++;
               buf += *it++;
               buf += *it;
           }
           else if ((*it & 0b11110000) == 0b11100000) {
               buf += *it++;
               buf += *it++;
               buf += *it;
           }
           else if ((*it & 0b11100000) == 0b11000000) {
               buf += *it++;
               buf += *it;
           }
           else {
               return QString(*it);
           }
           return QString(buf);
       }
    };
    for (auto it = data.begin(); it != data.end(); ++it) {
       if (*it == 0x19) {
           ++it;
           if (*it == 'F') {
               ++it;
               if (*it == '@') {
                   ++it;
                   loadAttr(it);
                   loadExt(it);
               }
               else {
                   loadAttr(it);
                   loadStd(it);
               }
           }
           else if (*it == 'B') {
               ++it;
               if (*it == '@')
                   loadBgExt(it);
               else
                   loadBgStd(it);
           }
           else if (*it == '*') {
               ++it;
               if (*it == '@') {
                   ++it;
                   loadAttr(it);
                   loadExt(it);
               }
               else {
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
                   }
                   else {
                       loadAttr(it);
                       loadBgStd(it);
                   }
               }
               else {
                   --it;
               }
           }
           else if (*it == '@') {
               ++it;
               loadExt(it);
           }
           else if (*it == 0x1C) {
               endColors();
           }
           else {
               loadStd(it);
           }
       }
       else if (*it == 0x1C) {
           endColors();
           endAttrs();
       }
       else if (*it == 0x1A) {
           loadAttr(it);
       }
       else if (*it == 0x1B) {
           clearAttr(it);
       }
       else if (*it) {
           result += getChar(it);
       }
    }
    endColors();
    endAttrs();

    result.prune();

    return result;
}

QString HData::toString() const {
    QString ret;

    ret += "HDATA\n";
    ret += "- PATH:\n";
    for (auto i : path) {
        ret += "\t" + i + "\n";
    }
    ret += "- KEYS:\n";
    for (auto i : keys) {
        ret += "\t" + i + "\n";
    }
    ret += "-VALUES:\n";
    for (auto i : data) {
        ret += "\t-PATH\n";
        ret += "\t\t";
        for (auto j : i.pointers) {
            ret += QString("%1").arg(j, 8, 16, QChar('0')) + " ";
        }
        ret += "\n";
        ret += "\t-OBJECTS\n";
        for (auto j : i.objects.keys()) {
            ret += QString("\t\t") + j + ": \"" + i.objects[j].toString() + "\"\n";
        }
        ret += "\n";
    }
    return ret;
}

} // namespace Protocol
