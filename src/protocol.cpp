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

static const QVector<QString> weechatColors {
    "default",
    "black",
    "dark gray",
    "dark red",
    "light red",
    "dark green",
    "light green",
    "brown",
    "yellow",
    "dark blue",
    "light blue",
    "dark magenta",
    "light magenta",
    "dark cyan",
    "light cyan",
    "gray",
    "white"
};

static const QVector<QString> extendedColors {
    "#000000", "#800000", "#008000", "#808000", "#000080", "#800080", "#008080", "#c0c0c0",
    "#808080", "#ff0000", "#00ff00", "#ffff00", "#0000ff", "#ff00ff", "#00ffff", "#ffffff",
    "#000000", "#00005f", "#000087", "#0000af", "#0000d7", "#0000ff", "#005f00", "#005f5f",
    "#005f87", "#005faf", "#005fd7", "#005fff", "#008700", "#00875f", "#008787", "#0087af",
    "#0087d7", "#0087ff", "#00af00", "#00af5f", "#00af87", "#00afaf", "#00afd7", "#00afff",
    "#00d700", "#00d75f", "#00d787", "00d7af ", "#00d7d7", "#00d7ff", "#00ff00", "#00ff5f",
    "#00ff87", "#00ffaf", "#00ffd7", "#00ffff", "#5f0000", "#5f005f", "#5f0087", "#5f00af",
    "#5f00d7", "#5f00ff", "#5f5f00", "#5f5f5f", "#5f5f87", "#5f5faf", "#5f5fd7", "#5f5fff",
    "#5f8700", "#5f875f", "#5f8787", "#5f87af", "#5f87d7", "#5f87ff", "#5faf00", "#5faf5f",
    "#5faf87", "#5fafaf", "#5fafd7", "#5fafff", "#5fd700", "#5fd75f", "#5fd787", "#5fd7af",
    "#5fd7d7", "#5fd7ff", "#5fff00", "#5fff5f", "#5fff87", "#5fffaf", "#5fffd7", "#5fffff",
    "#870000", "#87005f", "#870087", "#8700af", "#8700d7", "#8700ff", "#875f00", "#875f5f",
    "#875f87", "#875faf", "#875fd7", "#875fff", "#878700", "#87875f", "#878787", "#8787af",
    "#8787d7", "#8787ff", "#87af00", "#87af5f", "#87af87", "#87afaf", "#87afd7", "#87afff",
    "#87d700", "#87d75f", "#87d787", "#87d7af", "#87d7d7", "#87d7ff", "#87ff00", "#87ff5f",
    "#87ff87", "#87ffaf", "#87ffd7", "#87ffff", "#af0000", "#af005f", "#af0087", "#af00af",
    "#af00d7", "#af00ff", "#af5f00", "#af5f5f", "#af5f87", "#af5faf", "#af5fd7", "#af5fff",
    "#af8700", "#af875f", "#af8787", "#af87af", "#af87d7", "#af87ff", "#afaf00", "#afaf5f",
    "#afaf87", "#afafaf", "#afafd7", "#afafff", "#afd700", "#afd75f", "#afd787", "#afd7af",
    "#afd7d7", "#afd7ff", "#afff00", "#afff5f", "#afff87", "#afffaf", "#afffd7", "#afffff",
    "#d70000", "#d7005f", "#d70087", "#d700af", "#d700d7", "#d700ff", "#d75f00", "#d75f5f",
    "#d75f87", "#d75faf", "#d75fd7", "#d75fff", "#d78700", "#d7875f", "#d78787", "#d787af",
    "#d787d7", "#d787ff", "#d7af00", "#d7af5f", "#d7af87", "#d7afaf", "#d7afd7", "#d7afff",
    "#d7d700", "#d7d75f", "#d7d787", "#d7d7af", "#d7d7d7", "#d7d7ff", "#d7ff00", "#d7ff5f",
    "#d7ff87", "#d7ffaf", "#d7ffd7", "#d7ffff", "#ff0000", "#ff005f", "#ff0087", "#ff00af",
    "#ff00d7", "#ff00ff", "#ff5f00", "#ff5f5f", "#ff5f87", "#ff5faf", "#ff5fd7", "#ff5fff",
    "#ff8700", "#ff875f", "#ff8787", "#ff87af", "#ff87d7", "#ff87ff", "#ffaf00", "#ffaf5f",
    "#ffaf87", "#ffafaf", "#ffafd7", "#ffafff", "#ffd700", "#ffd75f", "#ffd787", "#ffd7af",
    "#ffd7d7", "#ffd7ff", "#ffff00", "#ffff5f", "#ffff87", "#ffffaf", "#ffffd7", "#ffffff",
    "#080808", "#121212", "#1c1c1c", "#262626", "#303030", "#3a3a3a", "#444444", "#4e4e4e",
    "#585858", "#626262", "#6c6c6c", "#767676", "#808080", "#8a8a8a", "#949494", "#9e9e9e",
    "#a8a8a8", "#b2b2b2", "#bcbcbc", "#c6c6c6", "#d0d0d0", "#dadada", "#e4e4e4", "#eeeeee"
};

bool Protocol::parse(QDataStream &s, Protocol::Char &r) {
    s.readRawData(&r.d, 1);
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::Integer &r) {
    s.setByteOrder(QDataStream::BigEndian);
    s >> r.d;
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::LongInteger &r) {
    quint8 length;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    r.d = buf.toLongLong();
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::String &r, bool canContainHtml) {
    uint32_t len;
    r.d.clear();
    s >> len;
    if (len == uint32_t(-1))
        r.d = QString();
    else if (len == 0)
        r.d = "";
    else if (len > 0) {
        QByteArray buf(len + 1, 0);
        s.readRawData(buf.data(), len);
        r.d = convertColorsToHtml(buf, canContainHtml);
    }
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::Buffer &r) {
    uint32_t len;
    r.d.clear();
    s >> len;
    if (len == 0)
        r.d = "";
    if (len > 0) {
        r.d = QByteArray(len, 0);
        s.readRawData(r.d.data(), len);
    }
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::Pointer &r) {
    quint8 length;;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    bool ok = false;
    r.d = buf.toULongLong(&ok, 16);
    if (!ok)
        return false;
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::Time &r) {
    quint8 length;
    s >> length;
    QByteArray buf((int) length + 1, 0);
    s.readRawData(buf.data(), length);
    r.d = buf;
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::HashTable &r) {
    char keyType[4] = { 0 }, valueType[4] = { 0 };
    s.readRawData(keyType, 3);
    if (QString(keyType) != "str") {
        qWarning() << "Hashtable currently supports only string keys";
        return false;
    }
    s.readRawData(valueType, 3);
    if (QString(valueType) != "str") {
        qWarning() << "Hashtable currently supports only string values";
        return false;
    }
    quint32 count = 0;
    s >> count;
    r.d.clear();
    for (quint32 i = 0; i < count; i++) {
        Protocol::String key, value;
        parse(s, key);
        parse(s, value);
        r.d.insert(key.d, value.d);
    }
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::HData &r) {
    Protocol::String hpath;
    Protocol::String keys;
    Protocol::Integer count;
    parse(s, hpath);
    parse(s, keys);
    parse(s, count);
    r.path = hpath.d.split("/");
    r.keys = keys.d.split(",");

    for (int i = 0; i < count.d; i++) {
        Protocol::HData::Item item;
        for (int j = 0; j < r.path.count(); j++) {
            Protocol::Pointer ptr;
            parse(s, ptr);
            item.pointers.append(ptr.d);
        }
        for (int j = 0; j < r.keys.count(); j++) {
            auto name = r.keys[j].split(":").first();
            auto type = r.keys[j].split(":").last();
            if (type == "int") {
                Protocol::Integer i;
                parse(s, i);
                item.objects[name] = QVariant::fromValue(i.d);
            }
            else if (type == "lon") {
                Protocol::LongInteger l;
                parse(s, l);
                item.objects[name] = QVariant::fromValue(l.d);
            }
            else if (type == "str" || type == "buf") {
                Protocol::String str;
                bool canContainHTML = false;
                if (name == "message" || name == "title" || name == "prefix")
                    canContainHTML = true;
                parse(s, str, canContainHTML);
                item.objects[name] = QVariant::fromValue(str.d);
            }
            else if (type == "arr") {
                char fieldType[4] = { 0 };
                s.readRawData(fieldType, 3);
                if (strcmp(fieldType, "int") == 0) {
                    Protocol::ArrayInt a;
                    parse(s, a);
                    item.objects[name] = QVariant::fromValue(a.d);
                }
                else if (strcmp(fieldType, "str") == 0) {
                    Protocol::ArrayStr a;
                    parse(s, a);
                    item.objects[name] = QVariant::fromValue(a.d);
                }
                else {
                    qCritical() << "Unhandled array item type:" << fieldType << "for field" << name;
                }
            }
            else if (type == "tim") {
                Protocol::Time t;
                parse(s, t);
                item.objects[name] = QVariant::fromValue(QDateTime::fromMSecsSinceEpoch(t.d.toLongLong() * 1000));
            }
            else if (type == "ptr") {
                Protocol::Pointer p;
                parse(s, p);
                item.objects[name] = QVariant::fromValue(p.d);
            }
            else if (type == "chr") {
                Protocol::Char c;
                parse(s, c);
                item.objects[name] = QVariant::fromValue(c.d);
            }
            else if (type == "htb") {
                Protocol::HashTable htb;
                parse(s, htb);
                item.objects[name] = QVariant::fromValue(htb.d);
            }
            else {
                qCritical() << "!!! Unhandled type:" << type << "for field" << name;
            }
        }
        r.data.append(item);
    }
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::ArrayInt &r) {
    uint32_t len;
    s >> len;
    for (uint32_t i = 0; i < len; i++) {
        Protocol::Integer num;
        parse(s, num);
        r.d.append(num.d);
    }
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::ArrayStr &r) {
    uint32_t len;
    s >> len;
    for (uint32_t i = 0; i < len; i++) {
        Protocol::String str;
        parse(s, str);
        r.d.append(str.d);
    }
    return true;
}

QString Protocol::convertColorsToHtml(const QByteArray &data, bool canContainHtml) {
    QString result;
    if (canContainHtml)
        result += "<html><body>";

    bool foreground = false;
    bool background = false;
    bool bold = false;
    bool reverse = false;
    bool italic = false;
    bool underline = false;
    bool keep = false;

    auto endColors = [&result, &foreground, &background]() {
        if (background) {
            background = false;
            result += "</span>";
        }
       if (foreground) {
           foreground = false;
           result += "</font>";
       }
    };
    auto endAttrs = [&result, &bold, &reverse, &italic, &underline, &keep]() {
       if (bold) {
           bold = false;
           result += "</b>";
       }
       if (reverse) {
           reverse = false;
           // TODO
       }
       if (italic) {
           italic = false;
           result += "</i>";
       }
       if (underline) {
           underline = false;
           result += "</u>";
       }
       if (keep) {
           // TODO
       }
    };
    auto loadAttr = [&result, &bold, &reverse, &italic, &underline, &keep](QByteArray::const_iterator &it) {
       while (true) {
           switch(*it) {
           case 0x01: // fallthrough // TODO what the fuck weechat
           case '*':
               if (bold)
                   break;
               bold = true;
               result += "<b>";
               break;
           case 0x02: // fallthrough
           case '!':
               if (reverse)
                   break;
               reverse = true;
               // TODO
               break;
           case 0x03: // fallthrough
           case '/':
               if (italic)
                   break;
               italic = true;
               result += "<i>";
               break;
           case 0x04: // fallthrough
           case '_':
               if (underline)
                   break;
               underline = true;
               result += "<u>";
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

    auto clearAttr = [&result, &bold, &reverse, &italic, &underline, &keep](QByteArray::const_iterator &it) {
       while (true) {
           switch(*it) {
           case 0x01: // fallthrough // TODO what the fuck weechat
           case '*':
               if (bold) {
                   bold = false;
                   result += "</b>";
               }
               break;
           case 0x02: // fallthrough
           case '!':
               if (reverse) {
                   reverse = false;
                   // TODO
               }
               break;
           case 0x03: // fallthrough
           case '/':
               if (italic) {
                   italic = false;
                   result += "</i>";
               }
               break;
           case 0x04: // fallthrough
           case '_':
               if (underline) {
                   underline = false;
                   result += "</u>";
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
    auto loadStd = [&result, &foreground](QByteArray::const_iterator &it) {
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
           result += "</font>";
           foreground = false;
       }
       if (code >= 0 && code < weechatColors.count()) {
           result += "<font color=\""+weechatColors[code]+"\">";
           foreground = true;
       }
    };
    auto loadExt = [&result, &foreground](QByteArray::const_iterator &it) {
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
           result += "</font>";
           foreground = false;
       }
       if (code >= 0 && code < extendedColors.count()) {
            result += "<font color=\""+extendedColors[code]+"\">";
            foreground = true;
       }
    };
    auto loadBgStd = [&result, &background](QByteArray::const_iterator &it) {
       while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|' || *it == ',' || *it == '~')
           ++it;
       int code = 0;
       for (int i = 0; i < 2; i++) {
           code *= 10;
           code += (*it) - '0';
           ++it;
       }
       --it;
       if (background)
           result += "</span>";
       if (code >= 0 && code < weechatColors.count()) {
           result += "<span style=\"background-color: "+weechatColors[code]+"\">";
           background = true;
       }
    };
    auto loadBgExt = [&result, &background](QByteArray::const_iterator &it) {
       while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|' || *it == ',' || *it == '~')
           ++it;
       int code = 0;
       for (int i = 0; i < 5; i++) {
           code *= 10;
           code += (*it) - '0';
           ++it;
       }
       --it;
       if (background)
           result += "</span>";
       if (code >= 0 && code < extendedColors.count()) {
           result += "<span style=\"background-color: "+extendedColors[code]+"\">";
           background = true;
       }
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
       else if (canContainHtml && (*it == '<'))
           result += "&lt;";
       else if (canContainHtml && (*it == '>'))
           result += "&gt;";
       else if (canContainHtml && (*it == '&'))
           result += "&amp;";
       else if (canContainHtml && (*it == '"'))
           result += "&quot;";
       else if (canContainHtml && (*it == '\''))
           result += "&apos;";
       else if (*it) {
           result += getChar(it);
       }
    }
    endColors();
    endAttrs();
    if (canContainHtml)
        result += "</body></html>";

    return result;
}

QString Protocol::HData::toString() const {
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
