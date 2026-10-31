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

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>

namespace WeeChatProtocol {
    using Char = char;
    using Integer = qint32;
    using LongInteger = qint64;
    // The difference betweehn plain and regular string is that plain is not user-facing,
    // so it never needs to be HTML-formatted (with colors, etc.).
    // This vastly improves performance when parsing HData keys.
    using PlainString = QString;
    using String = FormattedString;
    using Buffer = QByteArray;
    using Pointer = weechat_pointer_t;
    using Time = qint64;
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

    template <typename T> T parse(QDataStream& s, bool* ok = nullptr);

    template <> LITHCORE_EXPORT Char parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT Integer parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT LongInteger parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT PlainString parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT String parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT Buffer parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT Pointer parse(QDataStream& s, bool* ok);
    // The compiler thinks Time is the same class as LongInteger so no template specialization here
    // TODO: get rid of this template, it doesn't make much sense like this
    LITHCORE_EXPORT Time parseTime(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT HashTable parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT HData parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT ArrayInt parse(QDataStream& s, bool* ok);
    template <> LITHCORE_EXPORT ArrayStr parse(QDataStream& s, bool* ok);

    LITHCORE_EXPORT FormattedString convertColorsToHtml(const QByteArray& data);

    // Converts WeeChat Relay API (JSON) data into the same HData the binary protocol produces
    namespace Api {
        // Expects colors=weechat strings, which carry the same color codes as the binary protocol
        LITHCORE_EXPORT FormattedString stringFromJson(const QString& s);
        LITHCORE_EXPORT QDateTime dateFromJson(const QString& s);

        LITHCORE_EXPORT HData bufferHData(const QJsonObject& buffer);
        LITHCORE_EXPORT HData bufferHData(const QJsonArray& buffers);

        // withBufferKey: path is just the line and the buffer is in objects, like in _buffer_line_added
        LITHCORE_EXPORT HData lineHData(weechat_pointer_t bufferPtr, const QJsonObject& line, bool withBufferKey);
        LITHCORE_EXPORT HData lineHData(weechat_pointer_t bufferPtr, const QJsonArray& lines, bool withBufferKey);

        LITHCORE_EXPORT HData nickHData(weechat_pointer_t bufferPtr, const QJsonObject& nicklistRoot);
        // op is the binary protocol's _diff value: '+', '^' or '-'
        LITHCORE_EXPORT HData nickDiffHData(weechat_pointer_t bufferPtr, const QJsonObject& nickOrGroup, bool isGroup, char op);

        LITHCORE_EXPORT HData hotlistHData(const QJsonArray& hotlist);
    }  // namespace Api
};  // namespace WeeChatProtocol

Q_DECLARE_METATYPE(WeeChatProtocol::HData);
Q_DECLARE_METATYPE(WeeChatProtocol::HData*);

#endif  // PROTOCOL_H
