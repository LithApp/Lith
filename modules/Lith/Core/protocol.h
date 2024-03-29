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
#include "lithcore_export.h"

namespace WeeChatProtocol {
    using Char = char;
    using Integer = qint32;
    using LongInteger = qint64;
    using String = FormattedString;
    using Buffer = QByteArray;
    using Pointer = pointer_t;
    using Time = QString;
    using HashTable = StringMap;
    struct LITHCORE_EXPORT HData {
        struct Item {
            QList<Pointer> pointers;
            QMap<QString, QVariant> objects;
        };

        QStringList keys;
        QStringList path;
        QList<Item> data;

        QString toString() const;
    };
    using ArrayInt = QList<int>;
    using ArrayStr = QStringList;

    template <typename T> T parse(QDataStream& s, bool canContainHtml, bool* ok = nullptr);
    template <typename T> T parse(QDataStream& s, bool* ok = nullptr);

    template <> Char parse(QDataStream& s, bool* ok);
    template <> Integer parse(QDataStream& s, bool* ok);
    template <> LongInteger parse(QDataStream& s, bool* ok);
    template <> String parse(QDataStream& s, bool canContainHTML, bool* ok);
    template <> String parse(QDataStream& s, bool* ok);
    template <> Buffer parse(QDataStream& s, bool* ok);
    template <> Pointer parse(QDataStream& s, bool* ok);
    template <> Time parse(QDataStream& s, bool* ok);
    template <> HashTable parse(QDataStream& s, bool* ok);
    template <> HData parse(QDataStream& s, bool* ok);
    template <> ArrayInt parse(QDataStream& s, bool* ok);
    template <> ArrayStr parse(QDataStream& s, bool* ok);

    LITHCORE_EXPORT FormattedString convertColorsToHtml(const QByteArray& data, bool canContainHTML);
};  // namespace WeeChatProtocol

Q_DECLARE_METATYPE(WeeChatProtocol::HData);
Q_DECLARE_METATYPE(WeeChatProtocol::HData*);

#endif  // PROTOCOL_H
