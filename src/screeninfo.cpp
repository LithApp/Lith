#include "screeninfo.h"

#include <QApplication>
#include <QtGui/qpa/qplatformwindow.h>

ScreenInfo::ScreenInfo(QObject *parent) : QObject(parent)
{

}

qreal ScreenInfo::getBottomSafeAreaSize() {
    if (qApp->allWindows().count() > 0) {
        auto window = qApp->allWindows().first();
        QPlatformWindow *platformWindow = static_cast<QPlatformWindow *>(window->handle());
        QMargins margins = platformWindow->safeAreaMargins();
        return margins.bottom();
    }
    else {
        return 0.0;
    }
}
