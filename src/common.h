#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QDebug>

#include <cstdint>

#include "qmlobjectlist.h"

typedef uint64_t pointer_t;

#define STRINGIFY(x) #x

#define PROPERTY_NOSETTER(type, name, ...) \
    private: \
        Q_PROPERTY(type name READ name ## Get WRITE name ## Set NOTIFY name ## Changed) \
        type m_ ## name { __VA_ARGS__ }; \
    public: \
        type name ## Get () const { return m_ ## name; } \
        Q_SIGNAL void name ## Changed();

#define PROPERTY(type, name, ...) \
    PROPERTY_NOSETTER(type, name, __VA_ARGS__) \
    public: \
        void name ## Set (const type &o) { \
            if (m_ ## name != o) { \
                m_ ## name = o; \
                emit name ## Changed(); \
            } \
        }

#define PROPERTY_PTR(type, name, ...) \
    PROPERTY_NOSETTER(type*, name, __VA_ARGS__) \
    public: \
        void name ## Set (type *o) { \
            if (m_ ## name != o) { \
                m_ ## name = o; \
                emit name ## Changed(); \
            } \
        }

#define ALIAS(type, orig, alias) \
    Q_PROPERTY(type alias READ orig ## Get WRITE orig ## Set NOTIFY orig ## Changed)

#endif // COMMON_H
