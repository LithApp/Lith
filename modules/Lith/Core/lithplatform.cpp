#include "lithplatform.h"

LithPlatform::LithPlatform()
    : QObject() {
}

LithPlatform* LithPlatform::instance() {
    static LithPlatform* self = nullptr;
    if (!self) {
        self = new LithPlatform();
    }
    return self;
}

bool LithPlatform::isIos() {
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}

bool LithPlatform::isAndroid() {
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool LithPlatform::isWindows() {
#ifdef Q_OS_WINDOWS
    return true;
#else
    return false;
#endif
}

bool LithPlatform::isMacos() {
#ifdef Q_OS_MACOS
    return true;
#else
    return false;
#endif
}

bool LithPlatform::isLinux() {
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}

bool LithPlatform::isWasm() {
#ifdef Q_OS_WASM
    return true;
#else
    return false;
#endif
}

bool LithPlatform::isMobile() {
    return isIos() || isAndroid();
}

bool LithPlatform::isDesktop() {
    return isWindows() || isMacos() || isLinux() || isWasm();
}

bool LithPlatform::isUnknown() {
    return !isMobile() && !isDesktop();
}
