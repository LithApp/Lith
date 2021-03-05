// Lith
// Copyright (C) 2020 Martin Bříza
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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "common.h"

class Protocol {
public:
    struct Char { char d { 0 }; };
    struct Integer { int32_t d { 0 }; };
    struct LongInteger { int64_t d { 0 }; };
    struct String { QString d {}; };
    struct Buffer { QByteArray d {}; };
    struct Pointer { pointer_t d { 0 }; };
    struct Time { QString d {}; };
    struct HashTable { StringMap d {}; };
    struct HData {
        struct Item {
            QList<pointer_t> pointers;
            QMap<QString,QVariant> objects;
        };

        QStringList keys;
        QStringList path;
        QList<Item> data;

        QString toString() const;
    };
    struct ArrayInt { QList<int> d {}; };
    struct ArrayStr { QStringList d {}; };

    static bool parse(QDataStream &s, Char &r);
    static bool parse(QDataStream &s, Integer &r);
    static bool parse(QDataStream &s, LongInteger &r);
    static bool parse(QDataStream &s, String &r, bool canContainHTML = false);
    static bool parse(QDataStream &s, Buffer &r);
    static bool parse(QDataStream &s, Pointer &r);
    static bool parse(QDataStream &s, Time &r);
    static bool parse(QDataStream &s, HashTable &r);
    static bool parse(QDataStream &s, HData &r);
    static bool parse(QDataStream &s, ArrayInt &r);
    static bool parse(QDataStream &s, ArrayStr &r);

    static QString convertColorsToHtml(const QByteArray &data, bool canContainHTML);
};
Q_DECLARE_METATYPE(Protocol::HData);
Q_DECLARE_METATYPE(Protocol::HData*);
Q_DECLARE_METATYPE(Protocol::String);

#endif // PROTOCOL_H
