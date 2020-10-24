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

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QQuickStyle::setStyle("Material");
    QQmlApplicationEngine engine;
    qmlRegisterUncreatableType<BufferLine>("lith", 1, 0, "Line", "");
    qmlRegisterUncreatableType<BufferLineSegment>("lith", 1, 0, "LineSegment", "");
    qmlRegisterUncreatableType<Weechat>("lith", 1, 0, "Weechat", "");
    qmlRegisterUncreatableType<Nick>("lith", 1, 0, "Nick", "");
    qmlRegisterUncreatableType<Buffer>("lith", 1, 0, "Buffer", "");
    //qmlRegisterUncreatableType<LineModel>("lith", 1, 0, "LineModel", "");
    qmlRegisterUncreatableType<StuffManager>("lith", 1, 0, "StuffManager", "");
    qmlRegisterUncreatableType<ClipboardProxy>("lith", 1, 0, "ClipboardProxy", "");
    engine.rootContext()->setContextProperty("stuff", StuffManager::instance());
    engine.rootContext()->setContextProperty("weechat", Weechat::instance());
    engine.rootContext()->setContextProperty("clipboard", new ClipboardProxy());
    engine.rootContext()->setContextProperty("uploader", new Uploader());
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
