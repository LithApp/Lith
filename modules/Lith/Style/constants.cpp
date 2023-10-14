#include "constants.h"

#include "../Core/settings.h"
#include "../Core/lith.h"

FontSizes::FontSizes(QObject* parent)
    : m_settings(Lith::settingsGet()) {
    connect(m_settings, &Settings::baseFontPixelSizeChanged, this, &FontSizes::fontSizesChanged);
    connect(m_settings, &Settings::useBaseFontPixelSizeForMessagesChanged, this, &FontSizes::messageChanged);
    connect(m_settings, &Settings::messageFontPixelSizeChanged, this, &FontSizes::messageChanged);
    connect(this, &FontSizes::fontSizesChanged, this, [this]() {
        if (m_settings->useBaseFontPixelSizeForMessagesGet()) {
            emit messageChanged();
        }
    });
}

int FontSizes::tiny() const {
    return 0.75 * m_settings->baseFontPixelSizeGet();
}

int FontSizes::small() const {
    return 0.875 * m_settings->baseFontPixelSizeGet();
}

int FontSizes::regular() const {
    return m_settings->baseFontPixelSizeGet();
}

int FontSizes::medium() const {
    return 1.125 * m_settings->baseFontPixelSizeGet();
}

int FontSizes::large() const {
    return 1.25 * m_settings->baseFontPixelSizeGet();
}

int FontSizes::header() const {
    return 1.375 * m_settings->baseFontPixelSizeGet();
}

int FontSizes::message() const {
    if (m_settings->useBaseFontPixelSizeForMessagesGet()) {
        return regular();
    } else {
        return m_settings->messageFontPixelSizeGet();
    }
}
