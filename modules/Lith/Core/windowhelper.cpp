#include "windowhelper.h"
#include "lith.h"
#include "platform.h"

#include <QApplication>
#include <QPalette>
#include <QtMath>

#include <QtGui/qpa/qplatformwindow.h>

#include <cmath>

WindowHelper::WindowHelper()
    : QObject()
    , m_safeAreaMargins(new SafeAreaMargins(this)) {
    connect(this, &WindowHelper::darkThemeChanged, this, &WindowHelper::themeChanged);
    connect(this, &WindowHelper::useBlackChanged, this, &WindowHelper::themeChanged);

    QTimer::singleShot(0, this, &WindowHelper::init);
}

WindowHelper* WindowHelper::instance() {
    static WindowHelper self;
    return &self;
}

void WindowHelper::init() {
    if (Lith::settingsGet()->forceDarkThemeGet()) {
        m_darkTheme = true;
    } else if (Lith::settingsGet()->forceLightThemeGet()) {
        m_darkTheme = false;
    } else {
        m_darkTheme = detectSystemDarkStyle();
    }
    emit darkThemeChanged();
    if (m_darkTheme) {
        m_useBlack = Lith::settingsGet()->useTrueBlackWithDarkThemeGet();
    }

    qApp->setPalette(currentTheme().palette());

    connect(Lith::settingsGet(), &Settings::forceDarkThemeChanged, this, &WindowHelper::init);
    connect(Lith::settingsGet(), &Settings::forceLightThemeChanged, this, &WindowHelper::init);
    connect(Lith::settingsGet(), &Settings::useTrueBlackWithDarkThemeChanged, this, &WindowHelper::init);
}

const ColorTheme& WindowHelper::currentTheme() const {
    if (m_darkTheme) {
        if (m_useBlack) {
            return *blackTheme;
        }
        return *darkTheme;
    }
    return *lightTheme;
}

const ColorTheme& WindowHelper::inverseTheme() const {
    if (m_darkTheme) {
        return *lightTheme;
    }
    if (m_useBlack) {
        return *blackTheme;
    }
    return *darkTheme;
}

QString WindowHelper::currentThemeName() const {
    return m_darkTheme ? QStringLiteral("dark") : QStringLiteral("light");
}

SafeAreaMargins* WindowHelper::safeAreaMargins() {
    return m_safeAreaMargins;
}

void WindowHelper::updateSafeAreaMargins(QQuickWindow* window) {
    if (!window) {
        return;
    }
    if (Platform::instance()->desktop()) {
        bool isLandscape = window->width() > window->height();
        landscapeModeSet(isLandscape);
    }
    auto* platformWindow = static_cast<QPlatformWindow*>(window->handle());
    if (!platformWindow) {
        return;
    }
    QMargins margins = platformWindow->safeAreaMargins();
    m_safeAreaMargins->setMargins(margins);
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
    return qSqrt(
               ((textColor.red() * textColor.red()) * 0.299) + ((textColor.green() * textColor.green()) * 0.587) +
               ((textColor.blue() * textColor.blue()) * 0.114)
           ) > 128;
#endif
}

SafeAreaMargins::SafeAreaMargins(QObject* parent)
    : QObject(parent) {
}

void SafeAreaMargins::setMargins(QMarginsF margins) {
    leftSet(margins.left());
    rightSet(margins.right());
    topSet(margins.top());
    bottomSet(margins.bottom());
}
