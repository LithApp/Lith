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
    SETTING(QStringList, shortcutAutocomplete, {"Tab"})
    SETTING(QStringList, shortcutSwitchToNextBuffer, {"Alt+Right", "Alt+Down"})
    SETTING(QStringList, shortcutSwitchToPreviousBuffer, {"Alt+Left", "Alt+Up"})

public:
    Settings(QObject *parent = nullptr);
private:
    QSettings m_settings;
};

#endif // SETTINGS_H
