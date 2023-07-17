#include "notificationhandler.h"
#include "lith.h"

QString NotificationHandler::deviceTokenGet() const {
    return m_deviceToken;
}

void NotificationHandler::deviceTokenSet(const QString &value) {
    if (m_deviceToken != value) {
        m_deviceToken = value;
        emit deviceTokenChanged();
    }
}

bool NotificationHandler::validGet() const {
    return !deviceTokenGet().isEmpty();
}

NotificationHandler::NotificationHandler(Lith *parent)
    : QObject(parent)
{
    connect(this, &NotificationHandler::deviceTokenChanged, this, &NotificationHandler::validChanged);
}
