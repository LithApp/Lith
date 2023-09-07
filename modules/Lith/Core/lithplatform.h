#ifndef LITHPLATFORM_H
#define LITHPLATFORM_H

#include <QObject>
#include <QQmlEngine>

class LithPlatform : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool ios READ isIos CONSTANT)
    Q_PROPERTY(bool android READ isAndroid CONSTANT)
    Q_PROPERTY(bool windows READ isWindows CONSTANT)
    Q_PROPERTY(bool macos READ isMacos CONSTANT)
    Q_PROPERTY(bool linux READ isLinux CONSTANT)
    Q_PROPERTY(bool wasm READ isWasm CONSTANT)

    Q_PROPERTY(bool mobile READ isMobile CONSTANT)
    Q_PROPERTY(bool desktop READ isDesktop CONSTANT)

    Q_PROPERTY(bool unknown READ isUnknown CONSTANT)
public:
    static LithPlatform* instance();
    static LithPlatform* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        return instance();
    }

    static bool isIos();
    static bool isAndroid();
    static bool isWindows();
    static bool isMacos();
    static bool isLinux();
    static bool isWasm();
    static bool isMobile();
    static bool isDesktop();
    static bool isUnknown();

signals:

private:
    explicit LithPlatform();
};

#endif  // LITHPLATFORM_H
