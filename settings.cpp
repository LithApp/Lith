#include "settings.h"

#include <QMetaObject>
#include <QMetaProperty>

Settings::Settings(QObject *parent)
    : QObject(parent)
{
    QSettings s;
    auto mo = Settings::metaObject();
    for (int i = 0; i < mo->propertyCount(); i++) {
        auto property = mo->property(i);
        QString name = property.name();
        auto defaultValue = property.read(this);
        auto settingsValue = s.value(name, defaultValue);
        if (defaultValue != settingsValue)
            property.write(this, settingsValue);
    }
}
