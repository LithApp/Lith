#include "constants.h"

#include "../Core/settings.h"
#include "../Core/lith.h"

FontSizes::FontSizes(QObject* parent)
    : m_settings(Lith::settingsGet()) {
    connect(m_settings, &Settings::baseFontSizeChanged, this, &FontSizes::fontSizesChanged);
}

qreal FontSizes::tiny() const {
    return 0.75 * m_settings->baseFontSizeGet();
}

qreal FontSizes::small() const {
    return 0.875 * m_settings->baseFontSizeGet();
}

qreal FontSizes::regular() const {
    return 1.0 * m_settings->baseFontSizeGet();
}

qreal FontSizes::medium() const {
    return 1.125 * m_settings->baseFontSizeGet();
}

qreal FontSizes::large() const {
    return 1.25 * m_settings->baseFontSizeGet();
}

qreal FontSizes::header() const {
    return 1.375 * m_settings->baseFontSizeGet();
}
