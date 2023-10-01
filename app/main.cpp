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

#include "settings.h"
#include "lith.h"
#include "windowhelper.h"

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

#include "translator.h"

#include <QtQml/qqmlextensionplugin.h>
// NOLINTSTART
// Q_IMPORT_QML_PLUGIN(Lith_UIPlugin)
// Q_IMPORT_QML_PLUGIN(Lith_StylePlugin)
// Q_IMPORT_QML_PLUGIN(Lith_CorePlugin)
#ifdef Q_OS_WASM
Q_IMPORT_PLUGIN(QWasmIntegrationPlugin)
#endif
// NOLINTEND

int main(int argc, char* argv[]) {
    QCoreApplication::setOrganizationName("Lith");
    QCoreApplication::setOrganizationDomain("lith.app");
    QCoreApplication::setApplicationName("Lith");
    if (QStringLiteral(PROJECT_VERSION) == QStringLiteral("0.0.0")) {
        if (QStringLiteral(GIT_STATE).isEmpty()) {
            QCoreApplication::setApplicationVersion(QStringLiteral(
                "Unknown version, please report this to https://github.com/LithApp/Lith and mention your platform and where you got this package."
            ));
        } else {
            QCoreApplication::setApplicationVersion(QStringLiteral(GIT_STATE));
        }
    } else {
        QCoreApplication::setApplicationVersion(QStringLiteral(PROJECT_VERSION));
    }

    QApplication app(argc, argv);
    QCommandLineParser cmdLine;
    cmdLine.setApplicationDescription(
        QStringLiteral("                    ===== WeeChat relay client =====\n"
                       "Homepage: https://github.com/LithApp/Lith\n"
                       "There are no options now. Just run the app without any arguments to get the UI.")
    );
    cmdLine.addHelpOption();
    cmdLine.addVersionOption();
    cmdLine.process(app);

    app.setWindowIcon(QIcon(":/icon.png"));

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral(":/"));
    // This is used when installed, varies by platform
    engine.addImportPath(QStringLiteral(LITH_INSTALL_MODULEDIR));
    engine.addImportPath(QStringLiteral("./modules"));
    // This is used when running from the build folder only
    engine.addImportPath(QStringLiteral("../../../../modules"));
    engine.addImportPath(QStringLiteral("../../.."));
    engine.addImportPath(QStringLiteral("../modules"));

    // Initialize UI helpers and fonts
    WindowHelper::instance()->init();
    Settings::instance();
    Lith::instance();
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

    // Start the engine
    engine.load(QUrl(QLatin1String("qrc:/qt/qml/App/main.qml")));
    return app.exec();
}
