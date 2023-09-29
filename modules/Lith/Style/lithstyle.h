#ifndef LITHSTYLE_H
#define LITHSTYLE_H

#include "common.h"

#include <QQmlEngine>
#include <QtQuickControls2/qquickattachedpropertypropagator.h>
#include <QtQuick/private/qquickpalette_p.h>

#include <QColor>

class LithStyle;

class Border : public QObject {
    Q_OBJECT
    PROPERTY(qreal, width, 0.0)
    PROPERTY(QColor, color, Qt::transparent)
public:
    Border(LithStyle* parent);
};

class Theme : public QQuickPalette {
    Q_OBJECT
public:
    using QQuickPalette::QQuickPalette;
};

class LithStyle : public QQuickAttachedPropertyPropagator {
    Q_OBJECT
    QML_NAMED_ELEMENT(LithStyle)
    QML_ATTACHED(LithStyle)
    QML_UNCREATABLE("")

    PROPERTY_PTR(Border, border, new Border(this))
    PROPERTY(QColor, backgroundColor, Qt::transparent)

    Q_PROPERTY(QColor frameBorderColor READ frameBorderColor NOTIFY paletteChanged)
public:
    explicit LithStyle(QObject* parent = nullptr)
        : QQuickAttachedPropertyPropagator(parent) {
    }

    static LithStyle* qmlAttachedProperties(QObject* object) {
        return new LithStyle(object);
    }

    static void initialize() {
    }

    QColor frameBorderColor() const;
    void setFrameBorderColor(const QColor& newFrameBorderColor);

signals:
    void paletteChanged();

private:
    QColor m_frameBorderColor = {255, 0, 0};
};

#endif  // LITHSTYLE_H
