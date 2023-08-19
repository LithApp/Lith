#ifndef FORMATSTRINGSPLITTER_H
#define FORMATSTRINGSPLITTER_H

#include <QObject>
#include "common.h"

class FormatStringSplitter;
class Buffer;

class FormatStringVariable : public QObject {
    Q_OBJECT
    PROPERTY_CONSTANT(int, index)
    PROPERTY(QString, name)
    PROPERTY_READONLY_PRIVATESETTER(bool, valid, false)
public:
    FormatStringVariable(FormatStringSplitter* parent, int index);

    FormatStringSplitter* splitter();

private slots:
    void onNameChanged();
};

/**
 * @brief The FormatStringSplitter class
 * This class exists as a utilty to let users retrieve strings with properties from the Lith datamodel
 * In short, they can set a QString-like \p format percent-encoded string, which synchronizes the \p variables model to contain as many
 * fields as there are %N placeholders in the format. These variables have a \p FormatStringVariable::name property that has to be set by
 * the user and if this class contains allowedPropertyNames, it will be checked against it to set the \p FormatStringVariable::valid
 * property if it matches (or not). Check the \p Reflection class for a nice way to retrieve such list easily. Then, if you set the \p
 * dataSource property and all conditions are met, the \p formattedData property should be filled with a string with properties retrieved
 * from the object.
 * @example I realize this is stupid complex (and I'll probably remember how it works later too), so consider these input properties:
 *     format: "%1 ABC %2 %1"
 *     allowedPropertyNames: {"name", "number", "whatever"}
 *     variables[0].name: "number"
 *     variables[1].name: "name"
 *     dataSource: Buffer* { "name": "#lith", "number": 3, "whatever": "...", ... }
 * Now with this input, the output properties should look like this:
 *     count: 2                                       // there are 3 %N placeholders but they can repeat, same as in QString
 *     formatValid: true
 *     variablesValid: true
 *     errorString: ""
 *     stringList: {"%1 ABC %2 %1", "number", "name"} // this is useful for storage in Settings
 *     formattedData: "3 ABC #lith 3"
 * It essentially does the same thing as doing QString("%1 ABC %2 %1").arg(dataSource.number).arg(dataSource.name)
 *
 * @todo The signal connections and change handling is a MESS, I definitely left a few bugs in there
 * @todo It would be nice to allow simple operations with the variables, especially ternary operators
 */
class FormatStringSplitter : public QObject {
    Q_OBJECT
    PROPERTY(QString, format)
    // List of properties that are allowed - it would be nice to retrieve them from the object directly but it doesn't work when it's null
    PROPERTY(QStringList, allowedPropertyNames)
    // This object will be used for data retrieval for the final formattedData string
    PROPERTY_PTR(QObject, dataSource, nullptr)
    // This is the list of FormatStringVariable instances, its count will be based on how many placeholders are in the format string
    Q_PROPERTY(QmlObjectList* variables READ variables CONSTANT)
    //////// Calculated based on the data
    Q_PROPERTY(int count READ countGet NOTIFY countChanged)
    // There are some rules to the format (pretty much the same as QString) so this is computed based on that
    PROPERTY_READONLY_PRIVATESETTER(bool, formatValid, false)
    // Variables are valid if there's no allowedPropertyNames set or if all property names are present in allowedPropertyNames
    PROPERTY_READONLY_PRIVATESETTER(bool, variablesValid, false)
    PROPERTY_READONLY_PRIVATESETTER(QString, errorString)
    // This contains a "serialized" version of this class - first field in the stringlist is the format, followed by variables
    Q_PROPERTY(QStringList stringList READ stringList NOTIFY stringListChanged)
    // This property is the most important one: it will hold the data retrieved from the dataSource object,
    // formatted based on the format string and the variables, provided all conditions are met (format and variables are correct, dataSource
    // is not null, etc.)
    Q_PROPERTY(QString formattedData READ formattedDataGet NOTIFY formattedDataChanged)
public:
    explicit FormatStringSplitter(QObject* parent = nullptr);

    QmlObjectList* variables();
    int countGet() const;

    Q_INVOKABLE QString formattedDataGet();

    /**
     * @brief fromStringList This is deserialization, really, it will load the stringlist into this splitter
     * @param items, where first one will be loaded as the format string and other elements will be injected in the variables in the given
     * order
     */
    Q_INVOKABLE void fromStringList(const QStringList& items);
    /**
     * @brief stringList And this is serialization but it doesn't have to be called and is updated based on a signal automatically
     * @return list of strings where the first one will be the format and all strings after it will be variable strings
     */
    QStringList stringList() const;

    /**
     * @brief validate
     * @param name
     * @return true if \p allowedOptions contains \p name or if \p allowedOptions is empty
     */
    bool validate(const QString& name);

private slots:
    void onFormatChanged();
    void onVariableValidChanged();
    void onDataSourceChanged();

signals:
    void countChanged();
    void formattedDataChanged();
    void stringListChanged();

private:
    QmlObjectList* m_variables;
    QList<QMetaObject::Connection> m_dataSourceConnections;
};

#endif  // FORMATSTRINGSPLITTER_H
