#include "windowhelper.h"
#include "lith.h"
#include "lithplatform.h"

#include <QApplication>
#include <QPalette>
#include <QtMath>

#include <cmath>

WindowHelper::WindowHelper()
    : QObject()
    , m_changeSchemeTimer(new QTimer(this)) {
    connect(this, &WindowHelper::darkThemeChanged, this, &WindowHelper::themeChanged);
    connect(this, &WindowHelper::useBlackChanged, this, &WindowHelper::themeChanged);

    m_changeSchemeTimer->setInterval(50);
    m_changeSchemeTimer->setSingleShot(true);
    connect(m_changeSchemeTimer, &QTimer::timeout, this, &WindowHelper::changeScheme);
}

WindowHelper* WindowHelper::instance() {
    static WindowHelper* self = nullptr;
    if (!self) {
        self = new WindowHelper();
        self->init();
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
        darkThemeSet(true);
    } else if (Lith::settingsGet()->forceLightThemeGet()) {
        darkThemeSet(false);
    } else {
        darkThemeSet(m_systemPrefersDarkStyle);
    }
    emit darkThemeChanged();
    if (m_darkTheme) {
        useBlackSet(Lith::settingsGet()->useTrueBlackWithDarkThemeGet());
    } else {
        useBlackSet(false);
    }

    qApp->setPalette(currentTheme().palette());
}

void WindowHelper::handleWindowChange() {
    if (!m_window) {
        return;
    }
    if (LithPlatform::instance()->isDesktop()) {
        bool isLandscape = m_window->width() > m_window->height();
        landscapeModeSet(isLandscape);
    }
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

void WindowHelper::connectToWindow(QQuickWindow* window) {
    if (m_window == window) {
        return;
    }
    if (m_window) {
        disconnect(m_window, nullptr, this, nullptr);
    }
    m_window = window;
    if (m_window) {
        connect(window, &QQuickWindow::widthChanged, this, &WindowHelper::handleWindowChange);
        connect(window, &QQuickWindow::heightChanged, this, &WindowHelper::handleWindowChange);
    }
    handleWindowChange();
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
