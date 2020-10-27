#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QQmlEngine>
#include <QQuickStyle>

#include "weechat.h"
#include "uploader.h"
#include "clipboardproxy.h"
#include "datamodel.h"
#include "settings.h"
#include "lith.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setOrganizationName("Lith");
    QCoreApplication::setOrganizationDomain("ma.rtinbriza.cz");
    QCoreApplication::setApplicationName("Lith");

    QApplication app(argc, argv);


    QQuickStyle::setStyle("Material");
    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<BufferLine>("lith", 1, 0, "Line", "");
    qmlRegisterUncreatableType<BufferLineSegment>("lith", 1, 0, "LineSegment", "");
    qmlRegisterUncreatableType<Lith>("lith", 1, 0, "Lith", "");
    qmlRegisterUncreatableType<Weechat>("lith", 1, 0, "Weechat", "");
    qmlRegisterUncreatableType<Nick>("lith", 1, 0, "Nick", "");
    qmlRegisterUncreatableType<Buffer>("lith", 1, 0, "Buffer", "");
    //qmlRegisterUncreatableType<LineModel>("lith", 1, 0, "LineModel", "");
    qmlRegisterUncreatableType<ClipboardProxy>("lith", 1, 0, "ClipboardProxy", "");
    qmlRegisterUncreatableType<Settings>("lith", 1, 0, "Settings", "");
    engine.rootContext()->setContextProperty("lith", Lith::instance());
    engine.rootContext()->setContextProperty("clipboard", new ClipboardProxy());
    engine.rootContext()->setContextProperty("uploader", new Uploader());
    engine.rootContext()->setContextProperty("settings", Lith::instance()->settingsGet());
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
