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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QFont>
#include <QFontDatabase>
#include <QPalette>


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setOrganizationName("Lith");
    QCoreApplication::setOrganizationDomain("ma.rtinbriza.cz");
    QCoreApplication::setApplicationName("Lith");

    QQuickStyle::setStyle(":/style");

    QApplication app(argc, argv);
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

    Lith::instance();
    Lith::instance()->windowHelperGet()->init();

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
    qmlRegisterUncreatableType<WindowHelper>("lith", 1, 0, "WindowHelper", "");
    engine.rootContext()->setContextProperty("lith", Lith::instance());
    engine.rootContext()->setContextProperty("clipboardProxy", new ClipboardProxy());
    engine.rootContext()->setContextProperty("uploader", new Uploader());
    engine.rootContext()->setContextProperty("settings", Lith::instance()->settingsGet());
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
