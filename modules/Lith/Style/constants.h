#pragma once

#include <QObject>
#include <QQmlEngine>

class Settings;
class FontSizes : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(qreal tiny READ tiny NOTIFY fontSizesChanged)
    Q_PROPERTY(qreal small READ small NOTIFY fontSizesChanged)
    Q_PROPERTY(qreal regular READ regular NOTIFY fontSizesChanged)
    Q_PROPERTY(qreal medium READ medium NOTIFY fontSizesChanged)
    Q_PROPERTY(qreal large READ large NOTIFY fontSizesChanged)
    Q_PROPERTY(qreal header READ header NOTIFY fontSizesChanged)
public:
    explicit FontSizes(QObject* parent = nullptr);
    static FontSizes* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        return new FontSizes(qmlEngine);
    }

    qreal tiny() const;
    qreal small() const;
    qreal regular() const;
    qreal medium() const;
    qreal large() const;
    qreal header() const;
signals:
    void fontSizesChanged();

private:
    Settings* m_settings = nullptr;
};
