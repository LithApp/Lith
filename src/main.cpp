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
#include "windowhelper.h"
#include "util/formatstringsplitter.h"
#include "util/reflection.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QFont>
#include <QFontDatabase>
#include <QPalette>
#include <QMetaType>
#include <QIcon>

#include <QtQml/qqmlextensionplugin.h>
// NOLINTSTART
Q_IMPORT_QML_PLUGIN(LithUIPlugin)
Q_IMPORT_QML_PLUGIN(LithStylePlugin)
// NOLINTEND

int main(int argc, char* argv[]) {
    QCoreApplication::setOrganizationName("Lith");
    QCoreApplication::setOrganizationDomain("ma.rtinbriza.cz");
    QCoreApplication::setApplicationName("Lith");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:///");
    engine.addImportPath(":/qt/qml/LithUI/ui");
    engine.addImportPath(":/qt/qml/LithStyle/ui/LithStyle");
    QQuickStyle::setStyle("LithStyle");

    // Register types
    qRegisterMetaType<StringMap>();
    qRegisterMetaType<WeeChatProtocol::HData>();
    qRegisterMetaType<WeeChatProtocol::HData*>();
    qRegisterMetaType<FormattedString>();
    qRegisterMetaType<WeeChatProtocol::String>();
    qmlRegisterUncreatableType<FormattedString>("lith", 1, 0, "formattedString", "");
    QMetaType::registerConverter<FormattedString, QString>([](const FormattedString& s) {
        if (s.containsHtml()) {
            if (Lith::instance()->windowHelperGet()->lightThemeGet()) {
                return s.toHtml(*lightTheme);
            } else {
                return s.toHtml(*darkTheme);
            }
        }
        return s.toPlain();
    });
    qmlRegisterUncreatableType<ColorTheme>("lith", 1, 0, "colorTheme", "");
    qRegisterMetaType<ColorTheme>();
    qmlRegisterUncreatableType<BufferLine>("lith", 1, 0, "Line", "");
    qRegisterMetaType<BufferLine>();
    qmlRegisterUncreatableType<Lith>("lith", 1, 0, "Lith", "");
    qRegisterMetaType<Lith>();
    qmlRegisterUncreatableType<Nick>("lith", 1, 0, "Nick", "");
    qRegisterMetaType<Nick>();
    qmlRegisterUncreatableType<Buffer>("lith", 1, 0, "Buffer", "");
    qRegisterMetaType<Buffer>();
    // qmlRegisterUncreatableType<LineModel>("lith", 1, 0, "LineModel", "");
    qmlRegisterUncreatableType<ClipboardProxy>("lith", 1, 0, "ClipboardProxy", "");
    qmlRegisterUncreatableType<Settings>("lith", 1, 0, "Settings", "");
    qmlRegisterUncreatableType<Uploader>("lith", 1, 0, "Uploader", "");
    qmlRegisterUncreatableType<WindowHelper>("lith", 1, 0, "WindowHelper", "");
    qmlRegisterUncreatableType<BaseNetworkProxy>("lith", 1, 0, "NetworkProxy", "");
    qmlRegisterUncreatableType<ReplayProxy>("lith", 1, 0, "ReplayProxy", "");
    qmlRegisterUncreatableType<RecordProxy>("lith", 1, 0, "RecordProxy", "");
    qmlRegisterType<FormatStringSplitter>("lith", 1, 0, "FormatStringSplitter");
    qmlRegisterType<Reflection>("lith", 1, 0, "Reflection");

    // Initialize UI helpers and fonts
    Settings::instance();
    Lith::instance();
    Lith::instance()->windowHelperGet()->init();
    auto fontFamilyFromSettings = Lith::settingsGet()->baseFontFamilyGet();
#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
    QFont font("Menlo");
#else
    QFontDatabase::addApplicationFont(":/fonts/Inconsolata-Variable.ttf");
    QFont font("Inconsolata");
#endif
    if (fontFamilyFromSettings.length() != 0) {  // fontFamilyFromSettings could be NULL (unlikely) or empty (not so unlikely)
        font = QFont(fontFamilyFromSettings
        );  // if the font doesn't exist, it doesn't matter atm, Qt fallsback to a monospace font on our behalf
    }
    font.setKerning(false);
    font.setHintingPreference(QFont::PreferNoHinting);
    font.setStyleHint(QFont::Monospace);
    app.setFont(font);
    app.setFont(font, "monospace");

    // Inject properties and start the engine
    engine.rootContext()->setContextProperty("lith", Lith::instance());
    engine.rootContext()->setContextProperty("clipboardProxy", new ClipboardProxy());
    engine.rootContext()->setContextProperty("uploader", new Uploader());
    engine.rootContext()->setContextProperty("settings", Lith::settingsGet());
    engine.load(QUrl(QLatin1String("qrc:/qt/qml/LithUI/ui/main.qml")));

    QPixmap iconPixmap(":/icon.png");
    // Xorg didn't like the original 2k icon
    QIcon icon(iconPixmap.scaled(QSize(256, 256)));
    app.setWindowIcon(icon);
    return app.exec();
}
