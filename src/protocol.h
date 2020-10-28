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
    struct HashTable { QMap<QString, QString> d {}; };
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
    static bool parse(QDataStream &s, String &r);
    static bool parse(QDataStream &s, Buffer &r);
    static bool parse(QDataStream &s, Pointer &r);
    static bool parse(QDataStream &s, Time &r);
    static bool parse(QDataStream &s, HashTable &r);
    static bool parse(QDataStream &s, HData &r);
    static bool parse(QDataStream &s, ArrayInt &r);
    static bool parse(QDataStream &s, ArrayStr &r);

    static QString convertColorsToHtml(const QByteArray &data);
};

#endif // PROTOCOL_H
