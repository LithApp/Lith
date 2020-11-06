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

typedef uint64_t pointer_t;
Q_DECLARE_METATYPE(pointer_t);

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
