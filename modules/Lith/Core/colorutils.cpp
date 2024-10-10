
#include "colorutils.h"
#include "windowhelper.h"

ColorUtils::ColorUtils(QObject* parent)
    : QObject {parent} {
}

QColor ColorUtils::daytimeModeAdjust(QColor c, float ratio) {
    if (WindowHelper::instance()->darkThemeGet()) {
        auto darkened = darken(c, ratio);
        return saturate(darkened, ratio);
    } else {
        auto lightened = lighten(c, ratio);
        return saturate(lightened, 1.0 / ratio);
    }
}
