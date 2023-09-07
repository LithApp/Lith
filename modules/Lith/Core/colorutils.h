#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <QObject>
#include <QQmlEngine>
#include <QColor>

class ColorUtils : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit ColorUtils(QObject* parent = nullptr);
    static ColorUtils* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        return new ColorUtils(qmlEngine);
    }

    Q_INVOKABLE static QColor mixColors(QColor a, QColor b, qreal ratio) {
        return QColor::fromRgbF(
            a.redF() * ratio + b.redF() * (1 - ratio), a.greenF() * ratio + b.greenF() * (1 - ratio),
            a.blueF() * ratio + b.blueF() * (1 - ratio), a.alphaF() * ratio + b.alphaF() * (1 - ratio)
        );
    }
    Q_INVOKABLE static QColor setAlpha(QColor c, qreal alpha) {
        QColor modified = c;
        modified.setAlphaF(alpha);
        return modified;
    }
    Q_INVOKABLE static QColor darken(QColor c, qreal ratio) {
        qreal newLightness = qMax(0.0, qMin(1.0, c.lightnessF() / ratio));
        return QColor::fromHslF(c.hslHueF(), c.hslSaturationF(), newLightness);
    }
    Q_INVOKABLE static QColor lighten(QColor c, qreal ratio) {
        return darken(c, 1 / ratio);
    }
};

#endif  // COLORUTILS_H
