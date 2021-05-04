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
#include <QTimer>

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settings()
{
    // Code taken from qt-webassembly-examples

    // m_settings will be ready at some later point in time - when
    // QSettings::status() returns NoError. (apologies for the somewhat
    // convoluted std::function and lambda use below)
    auto settingsReady = [this](){
        auto mo = Settings::metaObject();
        for (int i = 0; i < mo->propertyCount(); i++) {
            auto property = mo->property(i);
            auto name = property.name();
            auto defaultValue = property.read(this);
            auto settingsValue = m_settings.value(name, defaultValue);
            if (defaultValue != settingsValue) {
                property.write(this, settingsValue);
            }
        }
        emit ready();
    };
    std::function<void(void)> *testSettingsReady = new std::function<void(void)>();
    *testSettingsReady = [=](){
        if (m_settings.status() == QSettings::NoError) {
            settingsReady();
            delete testSettingsReady;
        } else {
            QTimer::singleShot(10, *testSettingsReady);
        }
    };
    (*testSettingsReady)();

}
