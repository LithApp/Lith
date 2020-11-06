// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
// Copyright (C) 2020 Jakub Mach
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

#include "settings.h"

#include <QMetaObject>
#include <QMetaProperty>

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    QSettings s;
    auto mo = Settings::metaObject();
    for (int i = 0; i < mo->propertyCount(); i++) {
        auto property = mo->property(i);
        auto name = property.name();
        auto defaultValue = property.read(this);
        auto settingsValue = s.value(name, defaultValue);
        if (defaultValue != settingsValue)
            property.write(this, settingsValue);
    }
}
