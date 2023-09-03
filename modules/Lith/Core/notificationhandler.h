#ifndef NOTIFICATIONHANDLER_H
#define NOTIFICATIONHANDLER_H

#include "common.h"
#include "lithcore_export.h"

class Lith;
class LITHCORE_EXPORT NotificationHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool valid READ validGet NOTIFY validChanged)
    Q_PROPERTY(QString deviceToken READ deviceTokenGet WRITE deviceTokenSet NOTIFY deviceTokenChanged)
public:
    explicit NotificationHandler(Lith* parent = nullptr);

    bool validGet() const;
    QString deviceTokenGet() const;

    void deviceTokenSet(const QString& value);

private slots:
    void registerForNotifications();

signals:
    void bufferSelected(int bufferNumber);

    void validChanged();
    void deviceTokenChanged();

private:
    QString m_deviceToken;
};

#endif  // NOTIFICATIONHANDLER_H
