#include "reflection.h"
#include "lith.h"

#include <QMetaType>
#include <QMetaProperty>

Reflection::Reflection(QObject *parent)
    : QObject{parent}
{

}

const QString &Reflection::classNameGet() const {
    return m_className;
}

void Reflection::setClassName(const QString &name) {
    if (m_className == name) {
        return;
    }
    m_className = name;
    emit classNameChanged();
}

const QStringList &Reflection::stringPropertiesGet() const
{
    // The names are cached because they don't change
    static QMap<QString, QStringList> allowedPropertiesForClass;

    if (!allowedPropertiesForClass.contains(m_className)) {
        if (m_className.isEmpty()) {
            allowedPropertiesForClass[m_className] = QStringList();
        }
        else {
            auto mt = QMetaType::fromName(m_className.toLocal8Bit());
            if (mt.isValid()) {
                const auto *mo = mt.metaObject();
                if (mo) {
                    for (int i = 0; i < mo->propertyCount(); i++) {
                        auto property = mo->property(i);
                        QString name = property.name();
                        if (name == "objectName") {
                            continue;
                        }
                        if (QMetaType::canConvert(property.metaType(), QMetaType(QMetaType::QString))) {
                            allowedPropertiesForClass[m_className].append(name);
                        }
                    }
                }
                else {
                    Lith::instance()->log(Logger::Unexpected, QString("MetaObject for class %1 could not be accessed").arg(m_className));
                    allowedPropertiesForClass[m_className] = QStringList();
                }
            }
            else {
                Lith::instance()->log(Logger::Unexpected, QString("MetaType for class %1 could not be accessed").arg(m_className));
                allowedPropertiesForClass[m_className] = QStringList();
            }
        }
    }
    return allowedPropertiesForClass[m_className];
}
