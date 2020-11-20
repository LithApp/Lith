// Lith
// Copyright (C) 2020 Martin Bříza
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

#include "weechat.h"
#include "uploader.h"
#include "clipboardproxy.h"
#include "datamodel.h"
#include "settings.h"
#include "lith.h"
#include "screeninfo.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QFont>
#include <QFontDatabase>
#include <QPalette>

void setColorScheme(bool dark) {
    QPalette palette;


    auto darken = [](const QColor &color, qreal amount = 0.01) {
        qreal h, s, l, a;
        color.getHslF(&h, &s, &l, &a);

        qreal lightness = l - amount;
        if (lightness < 0) {
            lightness = 0;
        }

        return QColor::fromHslF(h, s, lightness, a);
    };

    auto desaturate = [](const QColor &color, qreal amount = 0.01) {
        qreal h, s, l, a;
        color.getHslF(&h, &s, &l, &a);

        qreal saturation = s - amount;
        if (saturation < 0) {
            saturation = 0;
        }
        return QColor::fromHslF(h, saturation, l, a);
    };

    auto lighten = [](const QColor &color, qreal amount = 0.01)
    {
        qreal h, s, l, a;
        color.getHslF(&h, &s, &l, &a);

        qreal lightness = l + amount;
        if (lightness > 1) {
            lightness = 1;
        }
        return QColor::fromHslF(h, s, lightness, a);
    };

    auto mix = [](const QColor &c1, const QColor &c2, qreal bias = 0.5)
    {
        auto mixQreal = [](qreal a, qreal b, qreal bias) {
            return a + (b - a) * bias;
        };

        if (bias <= 0.0) {
            return c1;
        }

        if (bias >= 1.0) {
            return c2;
        }

        if (std::isnan(bias)) {
            return c1;

        }

        qreal r = mixQreal(c1.redF(), c2.redF(), bias);
        qreal g = mixQreal(c1.greenF(), c2.greenF(), bias);
        qreal b = mixQreal(c1.blueF(), c2.blueF(), bias);
        qreal a = mixQreal(c1.alphaF(), c2.alphaF(), bias);

        return QColor::fromRgbF(r, g, b, a);
    };

    if (dark) {
        // Colors defined in GTK adwaita style in _colors.scss
        QColor base_color = lighten(desaturate(QColor("#241f31"), 1.0), 0.02);
        QColor text_color = QColor("white");
        QColor bg_color = darken(desaturate(QColor("#3d3846"), 1.0), 0.04);
        QColor fg_color = QColor("#eeeeec");
        QColor selected_bg_color = darken(QColor("#3584e4"), 0.2);
        QColor selected_fg_color = QColor("white");
        QColor osd_text_color = QColor("white");
        QColor osd_bg_color = QColor("black");
        QColor shadow = ("#dd000000");

        QColor backdrop_fg_color = mix(fg_color, bg_color);
        QColor backdrop_base_color = lighten(base_color, 0.01);
        QColor backdrop_selected_fg_color = mix(text_color, backdrop_base_color, 0.2);

        // This is the color we use as initial color for the gradient in normal state
        // Defined in _drawing.scss button(normal)
        QColor button_base_color = darken(bg_color, 0.01);

        QColor link_color = lighten(selected_bg_color, 0.2);
        QColor link_visited_color = lighten(selected_bg_color, 0.1);

        palette.setColor(QPalette::All,      QPalette::Window,          bg_color);
        palette.setColor(QPalette::All,      QPalette::WindowText,      fg_color);
        palette.setColor(QPalette::All,      QPalette::Base,            base_color);
        palette.setColor(QPalette::All,      QPalette::AlternateBase,   base_color);
        palette.setColor(QPalette::All,      QPalette::ToolTipBase,     osd_bg_color);
        palette.setColor(QPalette::All,      QPalette::ToolTipText,     osd_text_color);
        palette.setColor(QPalette::All,      QPalette::Text,            fg_color);
        palette.setColor(QPalette::All,      QPalette::Button,          button_base_color);
        palette.setColor(QPalette::All,      QPalette::ButtonText,      fg_color);
        palette.setColor(QPalette::All,      QPalette::BrightText,      text_color);

        palette.setColor(QPalette::All,      QPalette::Light,           lighten(button_base_color));
        palette.setColor(QPalette::All,      QPalette::Midlight,        mix(lighten(button_base_color), button_base_color));
        palette.setColor(QPalette::All,      QPalette::Mid,             mix(darken(button_base_color), button_base_color));
        palette.setColor(QPalette::All,      QPalette::Dark,            darken(button_base_color));
        palette.setColor(QPalette::All,      QPalette::Shadow,          shadow);

        palette.setColor(QPalette::All,      QPalette::Highlight,       selected_bg_color);
        palette.setColor(QPalette::All,      QPalette::HighlightedText, selected_fg_color);

        palette.setColor(QPalette::All,      QPalette::Link,            link_color);
        palette.setColor(QPalette::All,      QPalette::LinkVisited,     link_visited_color);


        QColor insensitive_fg_color = mix(fg_color, bg_color);
        QColor insensitive_bg_color = mix(bg_color, base_color, 0.4);

        palette.setColor(QPalette::Disabled, QPalette::Window,          insensitive_bg_color);
        palette.setColor(QPalette::Disabled, QPalette::WindowText,      insensitive_fg_color);
        palette.setColor(QPalette::Disabled, QPalette::Base,            base_color);
        palette.setColor(QPalette::Disabled, QPalette::AlternateBase,   base_color);
        palette.setColor(QPalette::Disabled, QPalette::Text,            insensitive_fg_color);
        palette.setColor(QPalette::Disabled, QPalette::Button,          insensitive_bg_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText,      insensitive_fg_color);
        palette.setColor(QPalette::Disabled, QPalette::BrightText,      text_color);

        palette.setColor(QPalette::Disabled, QPalette::Light,           lighten(insensitive_bg_color));
        palette.setColor(QPalette::Disabled, QPalette::Midlight,        mix(lighten(insensitive_bg_color), insensitive_bg_color));
        palette.setColor(QPalette::Disabled, QPalette::Mid,             mix(darken(insensitive_bg_color), insensitive_bg_color));
        palette.setColor(QPalette::Disabled, QPalette::Dark,            darken(insensitive_bg_color));
        palette.setColor(QPalette::Disabled, QPalette::Shadow,          shadow);

        palette.setColor(QPalette::Disabled, QPalette::Highlight,       selected_bg_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, selected_fg_color);

        palette.setColor(QPalette::Disabled, QPalette::Link,            link_color);
        palette.setColor(QPalette::Disabled, QPalette::LinkVisited,     link_visited_color);


        palette.setColor(QPalette::Inactive, QPalette::Window,          bg_color);
        palette.setColor(QPalette::Inactive, QPalette::WindowText,      backdrop_fg_color);
        palette.setColor(QPalette::Inactive, QPalette::Base,            backdrop_base_color);
        palette.setColor(QPalette::Inactive, QPalette::AlternateBase,   backdrop_base_color);
        palette.setColor(QPalette::Inactive, QPalette::Text,            backdrop_fg_color);
        palette.setColor(QPalette::Inactive, QPalette::Button,          button_base_color);
        palette.setColor(QPalette::Inactive, QPalette::ButtonText,      backdrop_fg_color);
        palette.setColor(QPalette::Inactive, QPalette::BrightText,      text_color);

        palette.setColor(QPalette::Inactive, QPalette::Light,           lighten(insensitive_bg_color));
        palette.setColor(QPalette::Inactive, QPalette::Midlight,        mix(lighten(insensitive_bg_color), insensitive_bg_color));
        palette.setColor(QPalette::Inactive, QPalette::Mid,             mix(darken(insensitive_bg_color), insensitive_bg_color));
        palette.setColor(QPalette::Inactive, QPalette::Dark,            darken(insensitive_bg_color));
        palette.setColor(QPalette::Inactive, QPalette::Shadow,          shadow);

        palette.setColor(QPalette::Inactive, QPalette::Highlight,       selected_bg_color);
        palette.setColor(QPalette::Inactive, QPalette::HighlightedText, backdrop_selected_fg_color);

        palette.setColor(QPalette::Inactive, QPalette::Link,            link_color);
        palette.setColor(QPalette::Inactive, QPalette::LinkVisited,     link_visited_color);
    }
    else {
        // Colors defined in GTK adwaita style in _colors.scss
        QColor base_color = QColor("white");
        QColor text_color = QColor("black");
        QColor bg_color = QColor("#f6f5f4");
        QColor fg_color = QColor("#2e3436");
        QColor selected_bg_color = QColor("#3584e4");
        QColor selected_fg_color = QColor("white");
        QColor osd_text_color = QColor("white");
        QColor osd_bg_color = QColor("black");
        QColor shadow = ("#dd000000");

        QColor backdrop_fg_color = mix(fg_color, bg_color);
        QColor backdrop_base_color = darken(base_color, 0.01);
        QColor backdrop_selected_fg_color = backdrop_base_color;

        // This is the color we use as initial color for the gradient in normal state
        // Defined in _drawing.scss button(normal)
        QColor button_base_color = darken(bg_color, 0.04);

        QColor link_color = darken(selected_bg_color, 0.1);
        QColor link_visited_color = darken(selected_bg_color, 0.2);

        palette.setColor(QPalette::All,      QPalette::Window,          bg_color);
        palette.setColor(QPalette::All,      QPalette::WindowText,      fg_color);
        palette.setColor(QPalette::All,      QPalette::Base,            base_color);
        palette.setColor(QPalette::All,      QPalette::AlternateBase,   base_color);
        palette.setColor(QPalette::All,      QPalette::ToolTipBase,     osd_bg_color);
        palette.setColor(QPalette::All,      QPalette::ToolTipText,     osd_text_color);
        palette.setColor(QPalette::All,      QPalette::Text,            fg_color);
        palette.setColor(QPalette::All,      QPalette::Button,          button_base_color);
        palette.setColor(QPalette::All,      QPalette::ButtonText,      fg_color);
        palette.setColor(QPalette::All,      QPalette::BrightText,      text_color);

        palette.setColor(QPalette::All,      QPalette::Light,           lighten(button_base_color));
        palette.setColor(QPalette::All,      QPalette::Midlight,        mix(lighten(button_base_color), button_base_color));
        palette.setColor(QPalette::All,      QPalette::Mid,             mix(darken(button_base_color), button_base_color));
        palette.setColor(QPalette::All,      QPalette::Dark,            darken(button_base_color));
        palette.setColor(QPalette::All,      QPalette::Shadow,          shadow);

        palette.setColor(QPalette::All,      QPalette::Highlight,       selected_bg_color);
        palette.setColor(QPalette::All,      QPalette::HighlightedText, selected_fg_color);

        palette.setColor(QPalette::All,      QPalette::Link,            link_color);
        palette.setColor(QPalette::All,      QPalette::LinkVisited,     link_visited_color);

        QColor insensitive_fg_color = mix(fg_color, bg_color);
        QColor insensitive_bg_color = mix(bg_color, base_color, 0.4);

        palette.setColor(QPalette::Disabled, QPalette::Window,          insensitive_bg_color);
        palette.setColor(QPalette::Disabled, QPalette::WindowText,      insensitive_fg_color);
        palette.setColor(QPalette::Disabled, QPalette::Base,            base_color);
        palette.setColor(QPalette::Disabled, QPalette::AlternateBase,   base_color);
        palette.setColor(QPalette::Disabled, QPalette::Text,            insensitive_fg_color);
        palette.setColor(QPalette::Disabled, QPalette::Button,          insensitive_bg_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText,      insensitive_fg_color);
        palette.setColor(QPalette::Disabled, QPalette::BrightText,      text_color);

        palette.setColor(QPalette::Disabled, QPalette::Light,           lighten(insensitive_bg_color));
        palette.setColor(QPalette::Disabled, QPalette::Midlight,        mix(lighten(insensitive_bg_color), insensitive_bg_color));
        palette.setColor(QPalette::Disabled, QPalette::Mid,             mix(darken(insensitive_bg_color), insensitive_bg_color));
        palette.setColor(QPalette::Disabled, QPalette::Dark,            darken(insensitive_bg_color));
        palette.setColor(QPalette::Disabled, QPalette::Shadow,          shadow);

        palette.setColor(QPalette::Disabled, QPalette::Highlight,       selected_bg_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, selected_fg_color);

        palette.setColor(QPalette::Disabled, QPalette::Link,            link_color);
        palette.setColor(QPalette::Disabled, QPalette::LinkVisited,     link_visited_color);


        palette.setColor(QPalette::Inactive, QPalette::Window,          bg_color);
        palette.setColor(QPalette::Inactive, QPalette::WindowText,      backdrop_fg_color);
        palette.setColor(QPalette::Inactive, QPalette::Base,            backdrop_base_color);
        palette.setColor(QPalette::Inactive, QPalette::AlternateBase,   backdrop_base_color);
        palette.setColor(QPalette::Inactive, QPalette::Text,            backdrop_fg_color);
        palette.setColor(QPalette::Inactive, QPalette::Button,          button_base_color);
        palette.setColor(QPalette::Inactive, QPalette::ButtonText,      backdrop_fg_color);
        palette.setColor(QPalette::Inactive, QPalette::BrightText,      text_color);

        palette.setColor(QPalette::Inactive, QPalette::Light,           lighten(insensitive_bg_color));
        palette.setColor(QPalette::Inactive, QPalette::Midlight,        mix(lighten(insensitive_bg_color), insensitive_bg_color));
        palette.setColor(QPalette::Inactive, QPalette::Mid,             mix(darken(insensitive_bg_color), insensitive_bg_color));
        palette.setColor(QPalette::Inactive, QPalette::Dark,            darken(insensitive_bg_color));
        palette.setColor(QPalette::Inactive, QPalette::Shadow,          shadow);

        palette.setColor(QPalette::Inactive, QPalette::Highlight,       selected_bg_color);
        palette.setColor(QPalette::Inactive, QPalette::HighlightedText, backdrop_selected_fg_color);

        palette.setColor(QPalette::Inactive, QPalette::Link,            link_color);
        palette.setColor(QPalette::Inactive, QPalette::LinkVisited,     link_visited_color);
    }

    qApp->setPalette(palette);
}

bool determineDarkStyle() {
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

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setOrganizationName("Lith");
    QCoreApplication::setOrganizationDomain("ma.rtinbriza.cz");
    QCoreApplication::setApplicationName("Lith");

    QQuickStyle::setStyle(":/style");

    QApplication app(argc, argv);
    setColorScheme(determineDarkStyle());
#ifdef WIN32
    QFontDatabase fdb;
    fdb.addApplicationFont(":/fonts/Inconsolata-Variable.ttf");
    QFont font("Inconsolata");
    font.setKerning(false);
    font.setHintingPreference(QFont::PreferNoHinting);
    font.setStyleHint(QFont::Monospace);
#else
    QFont font("Menlo");
#endif
    app.setFont(font);
    app.setFont(font, "monospace");

    QQmlApplicationEngine engine;
    qRegisterMetaType<StringMap>();
    qRegisterMetaType<Protocol::HData>();
    qRegisterMetaType<Protocol::HData*>();
    qmlRegisterUncreatableType<BufferLine>("lith", 1, 0, "Line", "");
    qmlRegisterUncreatableType<BufferLineSegment>("lith", 1, 0, "LineSegment", "");
    qmlRegisterUncreatableType<Lith>("lith", 1, 0, "Lith", "");
    qmlRegisterUncreatableType<Nick>("lith", 1, 0, "Nick", "");
    qmlRegisterUncreatableType<Buffer>("lith", 1, 0, "Buffer", "");
    //qmlRegisterUncreatableType<LineModel>("lith", 1, 0, "LineModel", "");
    qmlRegisterUncreatableType<ClipboardProxy>("lith", 1, 0, "ClipboardProxy", "");
    qmlRegisterUncreatableType<Settings>("lith", 1, 0, "Settings", "");
    qmlRegisterUncreatableType<Uploader>("lith", 1, 0, "Uploader", "");
    engine.rootContext()->setContextProperty("lith", Lith::instance());
    engine.rootContext()->setContextProperty("clipboardProxy", new ClipboardProxy());
    engine.rootContext()->setContextProperty("uploader", new Uploader());
    engine.rootContext()->setContextProperty("settings", Lith::instance()->settingsGet());
    engine.rootContext()->setContextProperty("currentTheme", determineDarkStyle() ? "dark" : "light");
    engine.rootContext()->setContextProperty("screenInfo", new ScreenInfo());
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    Lith::instance()->darkThemeSet(determineDarkStyle());

    return app.exec();
}
