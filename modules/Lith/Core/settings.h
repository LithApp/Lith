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
#include "defs/cmakedefs.h"
#include "lithcore_export.h"
#include <QDebug>
#include <QKeySequence>
#include <QSettings>

#include <QCoro/QCoroTask>
#include <optional>

#define SETTING(type, name, ...)                     \
    PROPERTY_NOSETTER(type, name, __VA_ARGS__)       \
public:                                              \
    bool name##Set(const type& o) {                  \
        if (m_##name != o) {                         \
            m_##name = o;                            \
            m_settings.setValue(STRINGIFY(name), o); \
            m_settings.sync();                       \
            emit name##Changed();                    \
            return true;                             \
        }                                            \
        return false;                                \
    }

#define SETTING_NOSETTER(type, name, ...) PROPERTY_NOSETTER(type, name, __VA_ARGS__)

#define DEPRECATED_SETTING(type, name, ...) PROPERTY(type, name, __VA_ARGS__)

#if LITH_FEATURE_KEYCHAIN
namespace QKeychain {
    class Job;
}
#endif

/*
 * USAGE:
 * just add a SETTING to the class header and it'll get exposed to
 * QML and C++ code through the Settings singleton object
 */
class LITHCORE_EXPORT Settings : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(bool isReady READ isReady NOTIFY readyChanged)

    static inline const QStringList c_hotlistDefaultFormat {
        QStringLiteral("%1: %2/%3"), QStringLiteral("short_name"), QStringLiteral("hotMessages"), QStringLiteral("unreadMessages")
    };

    SETTING(int, lastOpenBuffer, -1)
// TODO revisit this, it's really really awkward to have different defaults for font families
#if defined(Q_OS_MACOS)
    SETTING(QString, baseFontFamily, "Menlo")
#else
    SETTING(QString, baseFontFamily, QStringLiteral("Inconsolata"))
#endif
    SETTING(int, baseFontPixelSize, 14)
    SETTING(bool, useBaseFontPixelSizeForMessages, true)
    SETTING(int, messageFontPixelSize, 14)

    SETTING(bool, shortenLongUrls, true)
    SETTING(int, shortenLongUrlsThreshold, 50)
    SETTING(int, nickCutoffThreshold, -1)
    SETTING(QString, timestampFormat, QStringLiteral("hh:mm:ss"))
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

    SETTING(bool, encryptedCredentials, true)
    Q_PROPERTY(bool credentialEncryptionAvailable READ credentialEncryptionAvailable CONSTANT)
    static bool credentialEncryptionAvailable();

    SETTING_NOSETTER(QString, host)
    bool hostSet(QString newHost, bool force = false);
    SETTING_NOSETTER(int, port, 9001)
    bool portSet(int newPort, bool force = false);
    // passphrase is intentionally not stored in memory
    QCoro::Task<std::optional<QString>> passphraseGet();
    void passphraseUnset();
    void passphraseSet(QString passphrase);
    PROPERTY_READONLY_PRIVATESETTER(bool, hasPassphrase, false)
    SETTING_NOSETTER(bool, useEmptyPassphrase, false)
    void useEmptyPassphraseSet(bool useEmptyPassphrase);
    SETTING(bool, encrypted, true)
    SETTING(bool, allowSelfSignedCertificates, false)
    SETTING(bool, handshakeAuth, true)
    SETTING(bool, connectionCompression, true)
#ifndef __EMSCRIPTEN__
    SETTING(bool, useWebsockets, false)
#endif  // __EMSCRIPTEN__
    SETTING_NOSETTER(QString, websocketsEndpoint, QStringLiteral("weechat"))
    bool websocketsEndpointSet(QString newWebsocketsEndpoint, bool force = false);

    SETTING(bool, enableReadlineShortcuts, true)
    SETTING(QStringList, shortcutSearchBuffer, {QStringLiteral("Alt+G")})
    SETTING(QStringList, shortcutNicklist, {QStringLiteral("Alt+N")})
    SETTING(QStringList, shortcutAutocomplete, {QStringLiteral("Tab")})
    SETTING(QStringList, shortcutSwitchToNextBuffer, {QStringLiteral("Alt+Right"), QStringLiteral("Alt+Down")})
    SETTING(QStringList, shortcutSwitchToPreviousBuffer, {QStringLiteral("Alt+Left"), QStringLiteral("Alt+Up")})
    SETTING(QStringList, shortcutSwitchToBuffer1, {QStringLiteral("Alt+1")})
    SETTING(QStringList, shortcutSwitchToBuffer2, {QStringLiteral("Alt+2")})
    SETTING(QStringList, shortcutSwitchToBuffer3, {QStringLiteral("Alt+3")})
    SETTING(QStringList, shortcutSwitchToBuffer4, {QStringLiteral("Alt+4")})
    SETTING(QStringList, shortcutSwitchToBuffer5, {QStringLiteral("Alt+5")})
    SETTING(QStringList, shortcutSwitchToBuffer6, {QStringLiteral("Alt+6")})
    SETTING(QStringList, shortcutSwitchToBuffer7, {QStringLiteral("Alt+7")})
    SETTING(QStringList, shortcutSwitchToBuffer8, {QStringLiteral("Alt+8")})
    SETTING(QStringList, shortcutSwitchToBuffer9, {QStringLiteral("Alt+9")})
    SETTING(QStringList, shortcutSwitchToBuffer10, {QStringLiteral("Alt+0")})

    SETTING(bool, hotlistEnabled, true)
    SETTING(QStringList, hotlistFormat, c_hotlistDefaultFormat)
    SETTING(bool, hotlistCompact, true)
    SETTING(bool, showJoinPartQuitMessages, true)

    SETTING(bool, showBufferListOnStartup, false)
    SETTING(bool, platformBufferControlPosition, true)
    SETTING(bool, bufferListShowsOnlyBuffersWithNewMessages, false)
    SETTING(bool, bufferListGroupingByServer, false)
    SETTING(bool, bufferListAlphabeticalSorting, false)

    SETTING(bool, showInternalData, false)
    SETTING(bool, enableLogging, false)
    SETTING(bool, enableReplayRecording, false)

    SETTING(bool, enableNotifications, false)

    // This is hidden for now
    SETTING(bool, terminalLikeChat, true)

    SETTING(QString, imgurApiKey, QStringLiteral(IMGUR_API_KEY))

    // UNUSED SETTINGS
    // determine what to do about these later
    // Obsoleted by hotlistFormat
    DEPRECATED_SETTING(bool, hotlistShowUnreadCount, true)
    // END OF UNUSED SETTINGS

public:
    Q_PROPERTY(QStringList hotlistDefaultFormat MEMBER c_hotlistDefaultFormat CONSTANT)

public slots:
    void saveNetworkSettings(
        QString host, int port, bool encrypted, bool allowSelfSignedCertificates, QString passphrase, bool useEmptyPassphrase,
        bool handshakeAuth, bool connectionCompression, bool useWebsockets, QString websocketsEndpoint
    );

signals:
    void networkSettingsChanged();
    void readyChanged();

public:
    static Settings* instance();
    bool isReady() const;

private slots:
    ///
    /// \brief migrateCredentialEncryption
    /// This method will look at the encryptedCredentials variable and see if the
    /// data is stored in the appropriate place. If not, it will move it to where
    /// it belongs. It's used when switching between these states and to migrate
    /// users from the previous versions.
    ///
    QCoro::Task<> migrateCredentialEncryption();
    QCoro::Task<> migrate();

private:
    explicit Settings(QObject* parent = nullptr);
    QCoro::Task<std::optional<QString>> getSensitiveValue(QString key);
    std::optional<QString> getPlainValue(const QString& key);
    QCoro::Task<std::optional<QString>> getSecureValue(QString key, bool fallback);

    QCoro::Task<bool> setSensitiveValue(QString key, QString value);
    bool setPlainValue(const QString& key, const QString& value);
    QCoro::Task<bool> setSecureValue(QString key, QString value, bool fallback);

    QCoro::Task<bool> deleteSensitiveValue(QString key);
    bool deletePlainValue(const QString& key);
    QCoro::Task<bool> deleteSecureValue(QString key, bool fallback);

    QSettings m_settings;
    bool m_ready = false;
};

#endif  // SETTINGS_H
