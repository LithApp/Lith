#include "protocol.h"

#include "lith.h"

#include <QDataStream>
#include <QApplication>
#include <QDateTime>
#include <QAbstractEventDispatcher>

static const QMap<int, QString> weechatColors {
    { 0, "" },
    { 1, "black" },
    { 2, "dark gray" },
    { 3, "#8b0000" },
    { 4, "#ffcccb" },
    { 5, "dark green" },
    { 6, "light green" },
    { 7, "brown" },
    { 8, "yellow" },
    { 9, "#00008b" },
    { 10, "#add8e6" },
    { 11, "#8b008b" },
    { 12, "#ff80ff" },
    { 13, "#008b8b" },
    { 14, "#e0ffff" },
    { 15, "gray" },
    { 16, "white" }
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

bool Protocol::parse(QDataStream &s, Protocol::String &r) {
    uint32_t len;
    r.d.clear();
    s >> len;
    if (len == uint32_t(-1))
        r.d = QString();
    else if (len == 0)
        r.d = "";
    else if (len > 0) {
        /*
        int i;
        while ((i = buf.indexOf((char) 0x1a)) >= 0) {
            buf.replace(i, 2, "");
        }
        while ((i = buf.indexOf((char) 0x1b)) >= 0) {
            buf.replace(i, 2, "");
        }
        while ((i = buf.indexOf((char) 0x1c)) >= 0) {
            buf.replace(i, 1, "");
        }
        while ((i = buf.indexOf((char) 0x19)) >= 0) {
            if (buf[i+1] == 'F')
                buf.replace(i, 4, "");
            else
                buf.replace(i, 3, "");
        }
        r.d = buf;
        */
        QByteArray buf(len + 1, 0);
        s.readRawData(buf.data(), len);
        r.d.clear();
        bool foreground = false;
        bool background = false;
        bool bold = false;
        bool reverse = false;
        bool italic = false;
        bool underline = false;
        bool keep = false;
        auto endColors = [&r, &foreground, &background]() {
            if (foreground) {
                foreground = false;
                r.d += "</font>";
            }
            if (background) {
                background = false;
                r.d += "</span>";
            }
        };
        auto endAttrs = [&r, &bold, &reverse, &italic, &underline, &keep]() {
            if (bold) {
                bold = false;
                r.d += "</b>";
            }
            if (reverse) {
                reverse = false;
                // TODO
            }
            if (italic) {
                italic = false;
                r.d += "</i>";
            }
            if (underline) {
                underline = false;
                r.d += "</u>";
            }
            if (keep) {
                // TODO
            }
        };
        auto loadAttr = [&r, &bold, &reverse, &italic, &underline, &keep](QByteArray::iterator &it) {
            while (true) {
                switch(*it) {
                case '*':
                    if (bold)
                        break;
                    bold = true;
                    r.d += "<b>";
                    break;
                case '!':
                    if (reverse)
                        break;
                    reverse = true;
                    // TODO
                    break;
                case '/':
                    if (italic)
                        break;
                    italic = true;
                    r.d += "<i>";
                    break;
                case '_':
                    if (underline)
                        break;
                    underline = true;
                    r.d += "<u>";
                    break;
                case '|':
                    break;
                case '@':
                    break;
                case 0x1A:
                    break;
                case 0x19:
                    break;
                default:
                    --it;
                    return;
                }
                ++it;
            }
        };

        auto clearAttr = [&r, &bold, &reverse, &italic, &underline, &keep](QByteArray::iterator &it) {
            while (true) {
                switch(*it) {
                case '*':
                    if (bold) {
                        bold = false;
                        r.d += "</b>";
                    }
                    break;
                case '!':
                    if (reverse) {
                        reverse = false;
                        // TODO
                    }
                    break;
                case '/':
                    if (italic) {
                        italic = false;
                        r.d += "</i>";
                    }
                    break;
                case '_':
                    if (underline) {
                        underline = false;
                        r.d += "</u>";
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
        auto loadStd = [&r, &foreground](QByteArray::iterator &it) {
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
            if (foreground)
                r.d += "</font>";
            if (weechatColors.contains(code)) {
                r.d += "<font color=\""+weechatColors[code]+"\">";
                foreground = true;
            }
        };
        auto loadExt = [&r, &foreground](QByteArray::iterator &it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
                ++it;
            int code = 0;
            for (int i = 0; i < 5; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (foreground)
                r.d += "</font>";
            r.d += "<font color=\"green\">";
            foreground = true;
        };
        auto loadBgStd = [&r, &background](QByteArray::iterator &it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
                ++it;
            int code = 0;
            for (int i = 0; i < 2; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (background)
                r.d += "</span";
            r.d += "<span style=\"background-color: blue\">";
            background = true;
        };
        auto loadBgExt = [&r, &background](QByteArray::iterator &it) {
            while (*it == '@' || *it == '*' || *it == '!' || *it == '/' || *it == '_' || *it == '|')
                ++it;
            int code = 0;
            for (int i = 0; i < 5; i++) {
                code *= 10;
                code += (*it) - '0';
                ++it;
            }
            --it;
            if (background)
                r.d += "</span";
            r.d += "<span style=\"background-color: red\">";
            background = true;
        };
        auto getChar = [](QByteArray::iterator &it) -> QString {
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
        for (auto it = buf.begin(); it != buf.end(); ++it) {
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
                        loadExt(it);
                    }
                    if (*it == ',' || *it == '~')
                        ++it;
                    if (*it == '@') {
                        ++it;
                        loadAttr(it);
                        loadExt(it);
                    }
                    else {
                        loadAttr(it);
                        loadExt(it);
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
                ++it;
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
                r.d += getChar(it);
            }
        }
        endColors();
        endAttrs();
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
    r.d = buf.toULongLong(nullptr, 16);
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
    return true;
}

bool Protocol::parse(QDataStream &s, Protocol::HData &r) {
    //qCritical() << type;
    Protocol::String hpath;
    Protocol::String keys;
    Protocol::Integer count;
    parse(s, hpath);
    parse(s, keys);
    parse(s, count);
    //qCritical() << hpath.d << keys.d << count.d;
    for (int i = 0; i < count.d; i++) {
        qApp->eventDispatcher()->processEvents(QEventLoop::AllEvents);
        QStringList pathElems = hpath.d.split("/");
        QStringList arguments = keys.d.split(",");
        pointer_t parentPtr = 0;
        pointer_t stuffPtr = 0;

        pointer_t bufferPtr = 0;
        pointer_t linePtr = 0;
        pointer_t nickPtr = 0;
        pointer_t hotListPtr = 0;
        for (int i = 0; i < pathElems.count(); i++) { // TODO
            Protocol::Pointer ptr;
            parse(s, ptr);
            if (pathElems[i] == "buffer") {
                bufferPtr = ptr.d;
            }
            else if (pathElems[i] == "line_data") {
                linePtr = ptr.d;
            }
            else if (pathElems[i] == "nicklist_item"){
                nickPtr = ptr.d;
            }
            else if (pathElems[i] == "hotlist") {
                hotListPtr = ptr.d;
            }
            if (i == 2 && hpath.d != "buffer/lines/line/line_data") {
                qCritical() << "OMG got three stuff path";
                exit(1);
            }
        }
        if (hotListPtr != 0) {
            stuffPtr = hotListPtr;
        }
        else if (linePtr != 0) {
            stuffPtr = linePtr;
        }
        else if (bufferPtr != 0) {
            stuffPtr = bufferPtr;
        }
        if (nickPtr != 0) {
            stuffPtr = nickPtr;
            parentPtr = bufferPtr;
        }
        for (auto i : arguments) {
            qApp->eventDispatcher()->processEvents(QEventLoop::AllEvents);
            QStringList argument = i.split(":");
            QString name = argument[0];
            QString type = argument[1];
            if (pathElems.last() == "hotlist") {
                //qCritical() << pathElems << arguments;
            }
            if (type == "int") {
                Protocol::Integer i;
                parse(s, i);
                //qCritical() << name << ":" << i.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(i.d));
            }
            else if (type == "lon") {
                Protocol::LongInteger l;
                parse(s, l);
                //qCritical() << name << ":" << l.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(l.d));
            }
            else if (type == "str" || type == "buf") {
                Protocol::String str;
                parse(s, str);
                //qCritical () << name << ":" << str.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(str.d));
            }
            else if (type == "arr") {
                char type[4] = { 0 };
                s.readRawData(type, 3);
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                //qCritical() << name << ":" << QString(type);
                if (strcmp(type, "int") == 0) {
                    Protocol::ArrayInt a;
                    parse(s, a);
                    //qCritical() << name << ":" << a.d;
                    if (stuff && stuff->property(qPrintable(name)).isValid())
                        stuff->setProperty(qPrintable(name), QVariant::fromValue(a.d));
                }
                if (strcmp(type, "str") == 0) {
                    Protocol::ArrayStr a;
                    parse(s, a);
                    //qCritical() << name << ":" << a.d;
                    if (stuff && stuff->property(qPrintable(name)).isValid())
                        stuff->setProperty(qPrintable(name), QVariant::fromValue(a.d));
                }
            }
            else if (type == "tim") {
                Protocol::Time t;
                parse(s, t);
                //qCritical() << name << ":" << t.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(QDateTime::fromMSecsSinceEpoch(t.d.toLongLong() * 1000)));
            }
            else if (type == "ptr") {
                Protocol::Pointer p;
                parse(s, p);
                //qCritical() << name << ":" << p.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                QObject *otherStuff = Lith::instance()->getObject(p.d, "");
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(otherStuff));
            }
            else if (type == "chr") {
                Protocol::Char c;
                parse(s, c);
                //qCritical() << name << ":" << c.d;
                QObject *stuff = Lith::instance()->getObject(stuffPtr, pathElems.last(), parentPtr);
                if (stuff && stuff->property(qPrintable(name)).isValid())
                    stuff->setProperty(qPrintable(name), QVariant::fromValue(c.d));
            }
            else {
                //qCritical() << "Type was" << type;
            }
        }
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
