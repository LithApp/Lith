#pragma once

#include <QObject>
#include <QQmlEngine>

class Settings;
class FontSizes : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(int tiny READ tiny NOTIFY fontSizesChanged)
    Q_PROPERTY(int small READ small NOTIFY fontSizesChanged)
    Q_PROPERTY(int regular READ regular NOTIFY fontSizesChanged)
    Q_PROPERTY(int medium READ medium NOTIFY fontSizesChanged)
    Q_PROPERTY(int large READ large NOTIFY fontSizesChanged)
    Q_PROPERTY(int header READ header NOTIFY fontSizesChanged)

    Q_PROPERTY(int message READ message NOTIFY messageChanged)
public:
    explicit FontSizes(QObject* parent = nullptr);

    int tiny() const;
    int small() const;
    int regular() const;
    int medium() const;
    int large() const;
    int header() const;

    int message() const;
signals:
    void fontSizesChanged();
    void messageChanged();

private:
    Settings* m_settings = nullptr;
};

class ControlProperties : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(qreal buttonRadius READ buttonRadius CONSTANT)
public:
    static ControlProperties* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        Q_UNUSED(jsEngine)
        return new ControlProperties(qmlEngine);
    }

    static qreal buttonRadius() {
        return 4.0;
    }

private:
    explicit ControlProperties(QObject* parent = nullptr);
};
