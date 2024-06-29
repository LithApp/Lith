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
        Q_UNUSED(jsEngine)
        return new ColorUtils(qmlEngine);
    }

    Q_INVOKABLE static QColor mixColors(QColor a, QColor b, float ratio) {
        return QColor::fromRgbF(
            a.redF() * ratio + b.redF() * (1.0f - ratio), a.greenF() * ratio + b.greenF() * (1.0f - ratio),
            a.blueF() * ratio + b.blueF() * (1.0f - ratio), a.alphaF() * ratio + b.alphaF() * (1.0f - ratio)
        );
    }
    Q_INVOKABLE static QColor setAlpha(QColor c, float alpha) {
        QColor modified = c;
        modified.setAlphaF(alpha);
        return modified;
    }
    Q_INVOKABLE static QColor darken(QColor c, float ratio) {
        qreal newLightness = qMax(0.0f, qMin(1.0f, c.lightnessF() / ratio));
        return QColor::fromHslF(c.hslHueF(), c.hslSaturationF(), newLightness);
    }
    Q_INVOKABLE static QColor lighten(QColor c, float ratio) {
        return darken(c, 1.0f / ratio);
    }
};

#endif  // COLORUTILS_H
