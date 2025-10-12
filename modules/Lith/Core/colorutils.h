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

    struct HSLA {
        HSLA() = default;
        HSLA(float h, float s, float l, float a = 1.0)
            : h(h)
            , s(s)
            , l(l)
            , a(a) {
        }
        explicit HSLA(QColor c) {
            c.getHslF(&h, &s, &l, &a);
        }
        QColor toColor() const {
            return QColor::fromHslF(h, s, l, a);
        }
        float h = 0.0F;
        float s = 0.0F;
        float l = 0.0F;
        float a = 0.0F;
    };


    Q_INVOKABLE static QColor mixColors(QColor a, QColor b, float ratio) {
        return QColor::fromRgbF(
            (a.redF() * ratio) + (b.redF() * (1.0F - ratio)), (a.greenF() * ratio) + (b.greenF() * (1.0F - ratio)),
            (a.blueF() * ratio) + (b.blueF() * (1.0F - ratio)), (a.alphaF() * ratio) + (b.alphaF() * (1.0F - ratio))
        );
    }
    Q_INVOKABLE static QColor setAlpha(QColor c, float alpha) {
        c.setAlphaF(alpha);
        return c;
    }
    Q_INVOKABLE static QColor darken(QColor c, float ratio) {
        HSLA hsla(c);
        hsla.l = std::clamp(hsla.l / ratio, 0.0F, 1.0F);
        return hsla.toColor();
    }
    Q_INVOKABLE static QColor lighten(QColor c, float ratio) {
        return darken(c, 1.0F / ratio);
    }
    Q_INVOKABLE static QColor saturate(QColor c, float ratio) {
        HSLA hsla(c);
        hsla.s = std::clamp(hsla.s * ratio, 0.0F, 1.0F);
        return hsla.toColor();
    }
    // If used with light mode, this will make things lighter and vice versa.
    Q_INVOKABLE static QColor daytimeModeAdjust(QColor c, float ratio);
};

#endif  // COLORUTILS_H
