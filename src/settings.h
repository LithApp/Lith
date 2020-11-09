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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "common.h"
#include <QSettings>
#include <QDebug>
#include <QKeySequence>

#define SETTING(type, name, ...) \
    PROPERTY_NOSETTER(type, name, __VA_ARGS__) \
    public: \
        void name ## Set (const type &o) { \
            if (m_ ## name != o) { \
                m_ ## name = o; \
                QSettings s; \
                s.setValue(STRINGIFY(name), o); \
                s.sync(); \
                emit name ## Changed(); \
            } \
        }

/*
 * USAGE:
 * just add a SETTING to the class header and it'll get exposed to
 * QML and C++ code through the Settings singleton object
 */
class Settings : public QObject {
    Q_OBJECT
    SETTING(int, lastOpenBuffer, -1)
    SETTING(qreal, baseFontSize, 10)
    SETTING(bool, shortenLongUrls, true)
    SETTING(int, shortenLongUrlsThreshold, 50)

    SETTING(bool, showSendButton, false)
    SETTING(bool, showAutocompleteButton, true)
    SETTING(bool, showGalleryButton, true)

    SETTING(QString, host)
    SETTING(int, port, 9001)
    SETTING(bool, encrypted, true)
    SETTING(QString, passphrase)

    SETTING(bool, enableReadlineShortcuts, true)
    SETTING(QStringList, shortcutSearchBuffer, {"Alt+G"})
    SETTING(QStringList, shortcutNicklist, {"Alt+N"})
    SETTING(QStringList, shortcutAutocomplete, {"Tab"})
    SETTING(QStringList, shortcutSwitchToNextBuffer, {"Alt+Right", "Alt+Down"})
    SETTING(QStringList, shortcutSwitchToPreviousBuffer, {"Alt+Left", "Alt+Up"})

public:
    Settings(QObject *parent = nullptr);
private:
    QSettings m_settings;
};

#endif // SETTINGS_H
