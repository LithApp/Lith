#include "platform.h"

Platform::Platform()
    : QObject() {
}

Platform* Platform::instance() {
    static Platform self;
    return &self;
}

bool Platform::ios() {
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}

bool Platform::android() {
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool Platform::windows() {
#ifdef Q_OS_WINDOWS
    return true;
#else
    return false;
#endif
}

bool Platform::macos() {
#ifdef Q_OS_MACOS
    return true;
#else
    return false;
#endif
}

bool Platform::linux() {
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}

bool Platform::wasm() {
#ifdef Q_OS_WASM
    return true;
#else
    return false;
#endif
}

bool Platform::mobile() {
    return ios() || android();
}

bool Platform::desktop() {
    return windows() || macos() || linux() || wasm();
}

bool Platform::unknown() {
    return !mobile() && !desktop();
}
