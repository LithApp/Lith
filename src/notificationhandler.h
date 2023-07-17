#ifndef IOSNOTIFICATIONS_H
#define IOSNOTIFICATIONS_H

#include "common.h"


class Lith;
class NotificationHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool valid READ validGet NOTIFY validChanged)
    Q_PROPERTY(QString deviceToken READ deviceTokenGet WRITE deviceTokenSet NOTIFY deviceTokenChanged)
public:
    NotificationHandler(Lith *parent = nullptr);

    bool validGet() const;
    QString deviceTokenGet() const;

    void deviceTokenSet(const QString &value);

signals:
    void bufferSelected(int bufferNumber);

    void validChanged();
    void deviceTokenChanged();

private:
    QString m_deviceToken;
};

#endif // IOSNOTIFICATIONS_H
