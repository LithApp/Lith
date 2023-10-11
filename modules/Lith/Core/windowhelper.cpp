#include "windowhelper.h"
#include "lith.h"
#include "lithplatform.h"

#include <QApplication>
#include <QPalette>
#include <QtMath>

#include <QtGui/qpa/qplatformwindow.h>

#include <cmath>

WindowHelper::WindowHelper()
    : QObject()
    , m_safeAreaMargins(new SafeAreaMargins(this))
    , m_changeSchemeTimer(new QTimer(this)) {
    connect(this, &WindowHelper::darkThemeChanged, this, &WindowHelper::themeChanged);
    connect(this, &WindowHelper::useBlackChanged, this, &WindowHelper::themeChanged);

    m_changeSchemeTimer->setInterval(50);
    m_changeSchemeTimer->setSingleShot(true);
    connect(m_changeSchemeTimer, &QTimer::timeout, this, &WindowHelper::changeScheme);

    QTimer::singleShot(0, this, &WindowHelper::init);
}

WindowHelper* WindowHelper::instance() {
    static WindowHelper* self = nullptr;
    if (!self) {
        self = new WindowHelper();
    }
    return self;
}

void WindowHelper::init() {
    detectSystemDarkStyle();
    changeScheme();

    connect(Settings::instance(), &Settings::forceDarkThemeChanged, this, &WindowHelper::prepareToChangeScheme);
    connect(Settings::instance(), &Settings::forceLightThemeChanged, this, &WindowHelper::prepareToChangeScheme);
    connect(Settings::instance(), &Settings::useTrueBlackWithDarkThemeChanged, this, &WindowHelper::prepareToChangeScheme);
}

void WindowHelper::prepareToChangeScheme() {
    m_changeSchemeTimer->start();
}

void WindowHelper::changeScheme() {
    if (Lith::settingsGet()->forceDarkThemeGet()) {
        m_darkTheme = true;
    } else if (Lith::settingsGet()->forceLightThemeGet()) {
        m_darkTheme = false;
    } else {
        m_darkTheme = m_systemPrefersDarkStyle;
    }
    emit darkThemeChanged();
    if (m_darkTheme) {
        m_useBlack = Lith::settingsGet()->useTrueBlackWithDarkThemeGet();
    }

    qApp->setPalette(currentTheme().palette());
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
    if (LithPlatform::instance()->isDesktop()) {
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

void WindowHelper::detectSystemDarkStyle() {
#if WIN32
    QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    auto value = registry.value("AppsUseLightTheme");
    if (value.isValid() && value.canConvert<int>()) {
        m_systemPrefersDarkStyle = !value.toInt();
    } else {
        m_systemPrefersDarkStyle = false;
    }
#else
    const QColor textColor = QGuiApplication::palette().color(QPalette::Text);
    m_systemPrefersDarkStyle = qSqrt(
                                   ((textColor.red() * textColor.red()) * 0.299) + ((textColor.green() * textColor.green()) * 0.587) +
                                   ((textColor.blue() * textColor.blue()) * 0.114)
                               ) > 128;
#endif
}

SafeAreaMargins::SafeAreaMargins(QObject* parent)
    : QObject(parent) {
}

void SafeAreaMargins::setMargins(QMarginsF margins) {
    if (LITH_IS_DEBUG_BUILD && margins.isNull()) {
        return;
    }
    leftSet(margins.left());
    rightSet(margins.right());
    topSet(margins.top());
    bottomSet(margins.bottom());
}
