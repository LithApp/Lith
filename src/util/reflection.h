#ifndef REFLECTION_H
#define REFLECTION_H

#include <QObject>
#include <QQmlEngine>

/**
 * @brief The Reflection class
 * This class provides a way to access information about an internal data type.
 * The class itself has to be registered using qRegisterMetaType<>() (maybe Q_DECLARE_METATYPE is enough, didn't try it tbh)
 * You need to set the \p className property and then the other properties will contain names of properties that are convertible to string
 * (and other types may be added later)
 */
class Reflection : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString className READ classNameGet WRITE setClassName NOTIFY classNameChanged REQUIRED)
    Q_PROPERTY(QStringList stringProperties READ stringPropertiesGet NOTIFY classNameChanged)
public:
    explicit Reflection(QObject* parent = nullptr);

    const QString& classNameGet() const;
    void setClassName(const QString& name);

    const QStringList& stringPropertiesGet() const;
signals:
    void classNameChanged();

private:
    QString m_className;
};

#endif  // REFLECTION_H
