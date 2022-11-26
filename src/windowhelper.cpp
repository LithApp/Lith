#include "windowhelper.h"
#include "lith.h"

#include <QApplication>
#include <QPalette>
#include <QtMath>

#include <QtGui/qpa/qplatformwindow.h>

#include <math.h>

WindowHelper::WindowHelper(QObject *parent) : QObject(parent) {
    connect(this, &WindowHelper::darkThemeChanged, this, &WindowHelper::themeChanged);
    connect(this, &WindowHelper::useBlackChanged, this, &WindowHelper::themeChanged);
}

void WindowHelper::init() {
    if (Lith::instance()->settingsGet()->forceDarkThemeGet())
        m_darkTheme = true;
    else if (Lith::instance()->settingsGet()->forceLightThemeGet())
        m_darkTheme = false;
    else
        m_darkTheme = detectSystemDarkStyle();
    emit darkThemeChanged();
    if (m_darkTheme) {
        m_useBlack = Lith::instance()->settingsGet()->useTrueBlackWithDarkThemeGet();
    }

    qApp->setPalette(currentTheme().palette());

    connect(Lith::instance()->settingsGet(), &Settings::forceDarkThemeChanged, this, &WindowHelper::init);
    connect(Lith::instance()->settingsGet(), &Settings::forceLightThemeChanged, this, &WindowHelper::init);
    connect(Lith::instance()->settingsGet(), &Settings::useTrueBlackWithDarkThemeChanged, this, &WindowHelper::init);
}

const ColorTheme &WindowHelper::currentTheme() {
    if (m_darkTheme) {
        if (m_useBlack)
            return blackTheme;
        return darkTheme;
    }
    return lightTheme;
}

const ColorTheme &WindowHelper::inverseTheme() {
    if (m_darkTheme) {
        return lightTheme;
    }
    if (m_useBlack)
        return blackTheme;
    return darkTheme;
}

QVariantMap WindowHelper::getSafeAreaMargins(QQuickWindow *window) {
    QVariantMap result {{"top", 0.0}, {"bottom", 0.0}, {"left", 0.0}, {"right", 0.0}};
    if (!window) {
        return result;
    }
    QPlatformWindow *platformWindow = static_cast<QPlatformWindow *>(window->handle());
    if (!platformWindow) {
        return result;
    }
    QMargins margins = platformWindow->safeAreaMargins();
    result["top"] = margins.top();
    result["bottom"] = margins.bottom();
    result["left"] = margins.left();
    result["right"] = margins.right();
    return result;
}

bool WindowHelper::detectSystemDarkStyle() {
#if WIN32
    QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    auto value = registry.value("AppsUseLightTheme");
    if (value.isValid() && value.canConvert<int>()) {
        return !value.toInt();
    }
    return false;
#else
    const QColor textColor = QGuiApplication::palette().color(QPalette::Text);
    if (qSqrt(((textColor.red() * textColor.red()) * 0.299) +
              ((textColor.green() * textColor.green()) * 0.587) +
              ((textColor.blue() * textColor.blue()) * 0.114)) > 128)
        return true;
    return false;
#endif
}
