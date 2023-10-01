#include "colortheme.h"


QString ColorTheme::getIcon(const QString& name) {
    switch (m_group) {
        case DARK:
            return QStringLiteral("qrc:/navigation/dark/%1/.png").arg(name);
        case LIGHT:
        default:
            return QStringLiteral("qrc:/navigation/light/%2.png").arg(name);
    }
}

QColor ColorTheme::dim(const QColor& color) {
    return color.lighter();
}

QPalette ColorTheme::palette() const {
    auto mix = [](const QColor& a, const QColor& b, float ratio = 0.5) {
        return QColor::fromRgbF(
            a.redF() * ratio + b.redF() * (1.0F - ratio), a.greenF() * ratio + b.greenF() * (1.0F - ratio),
            a.blueF() * ratio + b.blueF() * (1.0F - ratio), a.alphaF() * ratio + b.alphaF() * (1.0F - ratio)
        );
    };

    // TODO very likely needs a bit of tweaking to differentiate button, window and base
    // Also toho this array has no business being QByteArray or StringList
    // Also move it to the style
    QColor windowText = QColor::fromString(m_weechatColors[0]);
    QColor base = (windowText == Qt::white) ? Qt::black : Qt::white;
    QColor window = (base == Qt::white)                                     ? QColor(0xc2, 0xbd, 0xb8)
                    : (QColor::fromString(m_weechatColors[1]) == Qt::black) ? QColor(0x8, 0x7, 0x6)
                                                                            : QColor::fromString(m_weechatColors[1]);
    ;  // TODO
    QColor button = mix(windowText, window, 0.15);
    QColor light = mix(windowText, button, 0.4);
    QColor dark = mix(window, windowText, 0.4);
    QColor mid = mix(light, dark);
    QColor text = windowText;
    QColor bright_text = text;
    QPalette palette {windowText, button, light, dark, mid, text, bright_text, base, window};
    palette.setBrush(QPalette::Highlight, QColor(0xcc, 0x33, 0x33));
    palette.setBrush(QPalette::HighlightedText, Qt::white);
    palette.setBrush(QPalette::AlternateBase, mix(base, windowText, 0.9));
    palette.setBrush(QPalette::Disabled, QPalette::Text, mix(text, window, 0.6));
    palette.setBrush(QPalette::Disabled, QPalette::Button, mix(button, window, 0.45));
    return palette;
}
