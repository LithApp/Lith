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
        migrate();
        m_ready = true;
        emit readyChanged();
    };
    std::function<void(void)> *testSettingsReady = new std::function<void(void)>();
    *testSettingsReady = [=, this](){
        if (m_settings.status() == QSettings::NoError) {
            settingsReady();
            delete testSettingsReady;
        } else {
            QTimer::singleShot(10, this, *testSettingsReady);
        }
    };
    (*testSettingsReady)();
}

void Settings::migrate() {
    // When migrating another variable, it may be possible to come up with a smarter way to do this.
    // I think checking that m_settings contains something that's not present in the metaobject should be enough
    if (hotlistShowUnreadCountGet() == false) {
        if (hotlistFormatGet() == c_hotlistDefaultFormat) {
            m_settings.remove("hotlistShowUnreadCount");
            m_settings.sync();
            hotlistFormatSet({"%1", "number"});
        }
    }
}

void Settings::saveNetworkSettings(const QString host, int port, bool encrypted, bool allowSelfSignedCertificates,
                                   const QString &passphrase, bool handshakeAuth, bool connectionCompression,
                                   bool useWebsockets, const QString &websocketsEndpoint) {
    bool changed = false;

    if (hostSet(host))
        changed = true;
    if (portSet(port))
        changed = true;
    if (encryptedSet(encrypted))
        changed = true;
    if (allowSelfSignedCertificatesSet(allowSelfSignedCertificates))
        changed = true;
    if (!passphrase.isEmpty() && passphraseSet(passphrase))
        changed = true;
    if (handshakeAuthSet(handshakeAuth))
        changed = true;
    if (connectionCompressionSet(connectionCompression))
        changed = true;
#ifndef __EMSCRIPTEN__
    if (useWebsocketsSet(useWebsockets))
        changed = true;
#endif
    if (websocketsEndpointSet(websocketsEndpoint))
        changed = true;

    if (changed)
        emit networkSettingsChanged();
}

bool Settings::isReady() const {
    return m_ready;
}

Settings *Settings::instance()
{
    static Settings self;
    return &self;
}
