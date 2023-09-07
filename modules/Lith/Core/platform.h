#ifndef PLATFORM_H
#define PLATFORM_H

#include <QObject>
#include <QQmlEngine>

class Platform : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool ios READ ios CONSTANT)
    Q_PROPERTY(bool android READ android CONSTANT)
    Q_PROPERTY(bool windows READ windows CONSTANT)
    Q_PROPERTY(bool macos READ macos CONSTANT)
    Q_PROPERTY(bool linux READ linux CONSTANT)
    Q_PROPERTY(bool wasm READ wasm CONSTANT)

    Q_PROPERTY(bool mobile READ mobile CONSTANT)
    Q_PROPERTY(bool desktop READ desktop CONSTANT)

    Q_PROPERTY(bool unknown READ unknown CONSTANT)
public:
    static Platform* instance();
    static Platform* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        return instance();
    }

    static bool ios();
    static bool android();
    static bool windows();
    static bool macos();
    static bool linux();
    static bool wasm();
    static bool mobile();
    static bool desktop();
    static bool unknown();

signals:

private:
    explicit Platform();
};

#endif  // PLATFORM_H
