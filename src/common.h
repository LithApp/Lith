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

#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QDebug>

#include <cstdint>

#include "qmlobjectlist.h"

#include "util/formattedstring.h"

#if !defined(LITH_PLATFORM_MOBILE) && !defined(LITH_PLATFORM_DESKTOP)
#  if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
#    define LITH_PLATFORM_MOBILE 1
#  else // let's assume all other platforms are desktop for simplicity
#    define LITH_PLATFORM_DESKTOP 1
#  endif
#endif // Q_OS_IOS || Q_OS_ANDROID


typedef QMap<QString, QString> StringMap;
Q_DECLARE_METATYPE(StringMap);

typedef uint64_t pointer_t;
Q_DECLARE_METATYPE(pointer_t);

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define STRINGIFY(x) #x

#define PROPERTY_CONSTANT(type, name, ...) \
private: \
    Q_PROPERTY(type name READ name ## Get CONSTANT) \
    type m_ ## name { __VA_ARGS__ }; \
    public: \
    const type& name ## Get () const { return m_ ## name; }

#define PROPERTY_READONLY(type, name, ...) \
    private: \
        Q_PROPERTY(type name READ name ## Get NOTIFY name ## Changed) \
        type m_ ## name { __VA_ARGS__ }; \
    public: \
        const type& name ## Get () const { return m_ ## name; } \
        Q_SIGNAL void name ## Changed();

#define PROPERTY_NOSETTER(type, name, ...) \
    private: \
        Q_PROPERTY(type name READ name ## Get WRITE name ## Set NOTIFY name ## Changed) \
        type m_ ## name { __VA_ARGS__ }; \
    public: \
        const type& name ## Get () const { return m_ ## name; } \
        Q_SIGNAL void name ## Changed();

#define SETTER_DEFINITION(type, name) \
    void name ## Set (const type &newValue) { \
            if (m_ ## name != newValue) { \
                m_ ## name = newValue; \
                emit name ## Changed(); \
        } \
    }

#define PROPERTY_READONLY_PRIVATESETTER(type, name, ...) \
private: \
    Q_PROPERTY(type name READ name ## Get NOTIFY name ## Changed) \
    type m_ ## name { __VA_ARGS__ }; \
    SETTER_DEFINITION(type, name) \
    public: \
    const type& name ## Get () const { return m_ ## name; } \
    Q_SIGNAL void name ## Changed();

#define PROPERTY(type, name, ...) \
    PROPERTY_NOSETTER(type, name, __VA_ARGS__) \
    public: \
    SETTER_DEFINITION(type, name)

#define PROPERTY_PTR(type, name, ...) \
private: \
    Q_PROPERTY(type* name READ name ## Get WRITE name ## Set NOTIFY name ## Changed) \
    type* m_ ## name { __VA_ARGS__ }; \
    public: \
    type* name ## Get () { return m_ ## name; } \
    const type* name ## Get () const { return m_ ## name; } \
    Q_SIGNAL void name ## Changed(); \
    public: \
        void name ## Set (type *newValue) { \
            if (m_ ## name != newValue) { \
                m_ ## name = newValue; \
                emit name ## Changed(); \
            } \
        }

#define ALIAS(type, orig, alias) \
    Q_PROPERTY(type alias READ orig ## Get WRITE orig ## Set NOTIFY orig ## Changed)
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif // COMMON_H
