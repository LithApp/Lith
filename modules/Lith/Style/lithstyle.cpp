#include "lithstyle.h"

Border::Border(LithStyle* parent)
    : QObject(parent) {
}


QColor LithStyle::frameBorderColor() const {
    return m_frameBorderColor;
}

void LithStyle::setFrameBorderColor(const QColor& newFrameBorderColor) {
    if (m_frameBorderColor == newFrameBorderColor) {
        return;
    }
    m_frameBorderColor = newFrameBorderColor;
    emit paletteChanged();
}
