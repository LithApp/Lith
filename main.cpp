#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "weechat.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("stuff", StuffManager::instance());
    engine.rootContext()->setContextProperty("weechat", Weechat::instance());
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
