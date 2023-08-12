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
    bool name ## Set (const type &o) { \
        if (m_ ## name != o) { \
            m_ ## name = o; \
            m_settings.setValue(STRINGIFY(name), o); \
            m_settings.sync(); \
            emit name ## Changed(); \
            return true; \
        } \
        return false; \
    }

#define DEPRECATED_SETTING(type, name, ...) \
    PROPERTY(type, name, __VA_ARGS__)

/*
 * USAGE:
 * just add a SETTING to the class header and it'll get exposed to
 * QML and C++ code through the Settings singleton object
 */
class Settings : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool isReady READ isReady NOTIFY readyChanged)

    static inline const QStringList c_hotlistDefaultFormat{"%1: %2/%3", "short_name", "hotMessages", "unreadMessages"};

    SETTING(int, lastOpenBuffer, -1)
#if defined(Q_OS_MACOS)
    SETTING(QString, baseFontFamily, "Menlo")
#else
    SETTING(QString, baseFontFamily, "Inconsolata")
#endif
    SETTING(qreal, baseFontSize, 10)
    SETTING(bool, shortenLongUrls, true)
    SETTING(int, shortenLongUrlsThreshold, 50)
    SETTING(int, nickCutoffThreshold, -1)
    SETTING(QString, timestampFormat, "hh:mm:ss")
    SETTING(bool, showDateHeaders, true)
    SETTING(int, messageSpacing, 0)

#if defined(LITH_PLATFORM_DESKTOP)
    SETTING(bool, scrollbarsOverlayContents, false)
#else
    SETTING(bool, scrollbarsOverlayContents, true)
#endif

    SETTING(bool, forceDarkTheme, false)
    SETTING(bool, forceLightTheme, false)
    SETTING(bool, useTrueBlackWithDarkTheme, false)

    SETTING(bool, showSendButton, false)
    SETTING(bool, showAutocompleteButton, true)
    SETTING(bool, showGalleryButton, true)
    SETTING(bool, showSearchButton, true)
    SETTING(bool, useLocalInputBar, true)

    SETTING(bool, muteVideosByDefault, true)
    SETTING(bool, loopVideosByDefault, true)

    SETTING(bool, showImageThumbnails, false)
    SETTING(bool, openLinksDirectly, false)
    SETTING(bool, openLinksDirectlyInBrowser, false)

    SETTING(QString, host)
    SETTING(int, port, 9001)
    SETTING(bool, encrypted, true)
    SETTING(bool, allowSelfSignedCertificates, false)
    SETTING(QString, passphrase)
    SETTING(bool, handshakeAuth, false)
    SETTING(bool, connectionCompression, true)
#ifndef __EMSCRIPTEN__
    SETTING(bool, useWebsockets, false)
#endif // __EMSCRIPTEN__
    SETTING(QString, websocketsEndpoint, "weechat")

    SETTING(bool, enableReadlineShortcuts, true)
    SETTING(QStringList, shortcutSearchBuffer, {"Alt+G"})
    SETTING(QStringList, shortcutNicklist, {"Alt+N"})
    SETTING(QStringList, shortcutAutocomplete, {"Tab"})
    SETTING(QStringList, shortcutSwitchToNextBuffer, {"Alt+Right"})
    SETTING(QStringList, shortcutSwitchToPreviousBuffer, {"Alt+Left"})
    SETTING(QStringList, shortcutSwitchToBuffer1, {"Alt+1"})
    SETTING(QStringList, shortcutSwitchToBuffer2, {"Alt+2"})
    SETTING(QStringList, shortcutSwitchToBuffer3, {"Alt+3"})
    SETTING(QStringList, shortcutSwitchToBuffer4, {"Alt+4"})
    SETTING(QStringList, shortcutSwitchToBuffer5, {"Alt+5"})
    SETTING(QStringList, shortcutSwitchToBuffer6, {"Alt+6"})
    SETTING(QStringList, shortcutSwitchToBuffer7, {"Alt+7"})
    SETTING(QStringList, shortcutSwitchToBuffer8, {"Alt+8"})
    SETTING(QStringList, shortcutSwitchToBuffer9, {"Alt+9"})
    SETTING(QStringList, shortcutSwitchToBuffer10, {"Alt+0"})

    SETTING(bool, hotlistEnabled, true)
    SETTING(QStringList, hotlistFormat, c_hotlistDefaultFormat)
    SETTING(bool, hotlistCompact, true)
    SETTING(bool, showJoinPartQuitMessages, true)

    SETTING(bool, showBufferListOnStartup, false)
    SETTING(bool, platformBufferControlPosition, true)

    SETTING(bool, showInternalData, false)
    SETTING(bool, enableLogging, false)
    SETTING(bool, enableReplayRecording, false)

    SETTING(bool, enableNotifications, false)

    // This is hidden for now
    SETTING(bool, terminalLikeChat, true)

    SETTING(QString, imgurApiKey, IMGUR_API_KEY)

    // UNUSED SETTINGS
    // determine what to do about these later
    DEPRECATED_SETTING(bool, hotlistShowUnreadCount, true) // Obsoleted by hotlistFormat
    // END OF UNUSED SETTINGS

public:
    Q_PROPERTY(QStringList hotlistDefaultFormat MEMBER c_hotlistDefaultFormat CONSTANT)

public slots:
    void saveNetworkSettings(const QString& host, int port, bool encrypted, bool allowSelfSignedCertificates,
                             const QString& passphrase, bool handshakeAuth, bool connectionCompression,
                             bool useWebsockets, const QString& websocketsEndpoint);

signals:
    void networkSettingsChanged();
    void readyChanged();

public:
    static Settings* instance();
    bool isReady() const;
private:
    explicit Settings(QObject *parent = nullptr);
    void migrate();
    QSettings m_settings;
    bool m_ready = false;
};

#endif // SETTINGS_H
