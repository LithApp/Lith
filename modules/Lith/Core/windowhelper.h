#ifndef WINDOWHELPER_H
#define WINDOWHELPER_H

#include "common.h"
#include "lithcore_export.h"

#include <QQuickWindow>

class WindowHelper;

class LITHCORE_EXPORT SafeAreaMargins : public QObject {
    Q_OBJECT
    PROPERTY_READONLY_PRIVATESETTER(qreal, left, 0.0)
    PROPERTY_READONLY_PRIVATESETTER(qreal, right, 0.0)
    PROPERTY_READONLY_PRIVATESETTER(qreal, top, 0.0)
    PROPERTY_READONLY_PRIVATESETTER(qreal, bottom, 0.0)
public:
    explicit SafeAreaMargins(QObject* parent = nullptr);

    void setMargins(QMarginsF margins);
};

class LITHCORE_EXPORT WindowHelper : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    PROPERTY_READONLY_PRIVATESETTER(bool, darkTheme, false)
    PROPERTY_READONLY_PRIVATESETTER(bool, useBlack, false)
    Q_PROPERTY(bool lightTheme READ lightThemeGet NOTIFY darkThemeChanged)
    Q_PROPERTY(ColorTheme currentTheme READ currentTheme NOTIFY darkThemeChanged)
    Q_PROPERTY(QString currentThemeName READ currentThemeName NOTIFY darkThemeChanged)
    Q_PROPERTY(SafeAreaMargins* safeAreaMargins READ safeAreaMargins CONSTANT)
    PROPERTY_READONLY_PRIVATESETTER(bool, landscapeMode, false)
public:
    static WindowHelper* instance();
    static WindowHelper* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        return instance();
    }

    bool lightThemeGet() const {
        return !darkThemeGet();
    }
    const ColorTheme& currentTheme() const;
    const ColorTheme& inverseTheme() const;
    QString currentThemeName() const;

    SafeAreaMargins* safeAreaMargins();

public slots:
    void updateSafeAreaMargins(QQuickWindow* window);

private slots:
    void init();
    void prepareToChangeScheme();
    void changeScheme();

signals:
    void themeChanged();

private:
    explicit WindowHelper();
    void detectSystemDarkStyle();

    SafeAreaMargins* m_safeAreaMargins = nullptr;
    QTimer* m_changeSchemeTimer = nullptr;
    bool m_systemPrefersDarkStyle = false;
};

#endif  // WINDOWHELPER_H
