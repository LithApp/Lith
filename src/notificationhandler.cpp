#include "notificationhandler.h"
#include "lith.h"

#include "iosnotifications.h"

QString NotificationHandler::deviceTokenGet() const {
    return m_deviceToken;
}

void NotificationHandler::deviceTokenSet(const QString &value) {
    if (m_deviceToken != value) {
        m_deviceToken = value;
        emit deviceTokenChanged();
    }
}

void NotificationHandler::registerForNotifications()
{
#ifdef Q_OS_IOS
    if (Lith::settingsGet()->enableNotificationsGet()) {
        iosRegisterForNotifications();
    }
    else {
        iosUnregisterForNotifications();
    }
#endif
}

bool NotificationHandler::validGet() const {
    return !deviceTokenGet().isEmpty();
}

NotificationHandler::NotificationHandler(Lith *parent)
    : QObject(parent)
{
    if (Lith::settingsGet()->isReady()) {
        QTimer::singleShot(0, this, &NotificationHandler::registerForNotifications);
    }
    connect(Lith::settingsGet(), &Settings::enableNotificationsChanged, this, &NotificationHandler::registerForNotifications);
    connect(this, &NotificationHandler::deviceTokenChanged, this, &NotificationHandler::validChanged);
}
