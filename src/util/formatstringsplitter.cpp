#include "formatstringsplitter.h"

#include <QRegularExpression>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaType>
#include <QDebug>
#include <algorithm>

#include "lith.h"

static const QRegularExpression formatPlaceholderRegexp("[%]([0-9]+)");

FormatStringSplitter::FormatStringSplitter(QObject *parent)
    : QObject{parent}
    , m_variables(QmlObjectList::create<FormatStringVariable>(this))
{
    connect(this, &FormatStringSplitter::formatChanged, this, &FormatStringSplitter::onFormatChanged);
    connect(this, &FormatStringSplitter::variablesValidChanged, this, &FormatStringSplitter::formattedDataChanged);
    connect(this, &FormatStringSplitter::formatValidChanged, this, &FormatStringSplitter::formattedDataChanged);
    connect(this, &FormatStringSplitter::dataSourceChanged, this, &FormatStringSplitter::formattedDataChanged);
    connect(this, &FormatStringSplitter::formatChanged, this, &FormatStringSplitter::stringListChanged);
    connect(this, &FormatStringSplitter::countChanged, this, &FormatStringSplitter::stringListChanged);
}

QmlObjectList *FormatStringSplitter::variables() {
    return m_variables;
}

int FormatStringSplitter::countGet() const {
    return m_variables->count();
}

QString FormatStringSplitter::formattedDataGet() {
    if (!m_dataSource) {
        return QString();
    }
    if (!formatValidGet() || !variablesValidGet()) {
        return QString();
    }

    QString result(formatGet());

    auto mo = m_dataSource->metaObject();
    for (int i = 0; i < m_variables->count(); i++) {
        auto var = m_variables->get<FormatStringVariable>(i);
        if (!var) {
            continue;
        }
        if (!validate(var->nameGet())) {
            Lith::instance()->log(Logger::FormatSplitter, QString("FormatStringSplitter::formattedData was called with invalid variable (%1), inserting N/A").arg(var->nameGet()));
            result = result.arg("N/A");
            continue;
        }
        auto propertyIndex = mo->indexOfProperty(qPrintable(var->nameGet()));
        if (propertyIndex <= 0) {
            Lith::instance()->log(Logger::FormatSplitter, QString("Property %1 was not found in this object").arg(var->nameGet()));
            continue;
        }
        auto mp = mo->property(propertyIndex);
        auto propertyVariant = mp.read(m_dataSource);
        if (!propertyVariant.isValid()) {
            continue;
        }
        if (!propertyVariant.canConvert<QString>()) {
            Lith::instance()->log(Logger::FormatSplitter, QString("Not sure how but could not convert property %1 to QString").arg(var->nameGet()));
            continue;
        }
        QString propertyString = propertyVariant.toString();
        result = result.arg(propertyString);
    }

    return result;
}

void FormatStringSplitter::fromStringList(const QStringList &items) {
    bool first = true;
    int varIndex = 0;
    for (auto &i : items) {
        if (first) {
            formatSet(i);
        }
        else {
            auto variable = m_variables->get<FormatStringVariable>(varIndex);
            if (!variable) {
                Lith::instance()->log(Logger::FormatSplitter, QString("FormattedStringSplitter contains an invalid object at position %1").arg(i));
                varIndex++;
                continue;
            }
            variable->nameSet(i);
            varIndex++;
        }
        first = false;
    }
}

QStringList FormatStringSplitter::stringList() const {
    QStringList result;
    result.append(formatGet());
    for (int i = 0; i < m_variables->count(); i++) {
        auto variable = m_variables->get<FormatStringVariable>(i);
        if (!variable) {
            Lith::instance()->log(Logger::FormatSplitter, QString("FormattedStringSplitter contains an invalid object at position %1").arg(i));
            continue;
        }
        result.append(variable->nameGet());
    }
    return result;
}

bool FormatStringSplitter::validate(const QString &name) {
    if (m_allowedPropertyNames.isEmpty())
        return true;
    return m_allowedPropertyNames.contains(name);
}

void FormatStringSplitter::onFormatChanged() {
    // Somehow it feels this method should be much shorter
    QSet<int> numberSet;
    auto matches = formatPlaceholderRegexp.globalMatch(m_format);

    for (auto &match : matches) {
        if (match.capturedTexts().size() == 2) {
            bool ok = false;
            auto number = match.capturedView(1).toInt(&ok);
            if (ok) {
                numberSet.insert(number);
            }
            else {
                errorStringSet(tr("Format string %1 is not convertible to number").arg(match.capturedView(0)));
                formatValidSet(false);
                return;
            }
        }
        else {
            errorStringSet(tr("Format string %1 is invalid").arg(match.capturedTexts().join(",")));
            formatValidSet(false);
            return;
        }
    }

    QList<int> numberList(numberSet.begin(), numberSet.end());
    std::sort(numberList.begin(), numberList.end());
    if (numberList.isEmpty()) {
        errorStringSet("You need to enter at least one format string");
        formatValidSet(false);
        return;
    }
    else if (*numberList.begin() != 1) {
        errorStringSet(QString("Format strings need to start at 1, currently it is: %1").arg(*numberList.begin()));
        formatValidSet(false);
        return;
    }
    else {
        int previous = 0;
        for (auto &number : numberList) {
            if (number != previous + 1) {
                errorStringSet(QString("Format string numbers need to be consecutive (%1 came after %2)").arg(number).arg(previous));
                formatValidSet(false);
                return;
            }
            previous = number;
        }
    }

    errorStringSet(QString());
    int newCount = numberList.size();
    int oldCount = m_variables->count();
    while (m_variables->count() < newCount) {
        auto *var = new FormatStringVariable(this, m_variables->count());
        connect(var, &FormatStringVariable::validChanged, this, &FormatStringSplitter::onVariableValidChanged);
        connect(var, &FormatStringVariable::nameChanged, this, [this]() {
            if (variablesValidGet() && formatValidGet()) {
                emit formattedDataChanged();
            }
        });
        connect(this, &FormatStringSplitter::countChanged, this, &FormatStringSplitter::stringListChanged);
        m_variables->append(var);
    }
    while (m_variables->count() > newCount) {
        m_variables->removeLast();
    }
    bool allValid = true;
    for (int i = 0; i < m_variables->count(); i++) {
        auto variable = m_variables->get<FormatStringVariable>(i);
        if (!variable) {
            continue;
        }
        if (!variable->validGet()) {
            allValid = false;
            break;
        }
    }
    variablesValidSet(allValid);
    if (newCount != oldCount)
        emit countChanged();
    formatValidSet(true);
    emit formattedDataChanged();
}

void FormatStringSplitter::onVariableValidChanged() {
    if (m_variables->count() == 0) {
        variablesValidSet(false);
        return;
    }
    for (int i = 0; i < m_variables->count(); i++) {
        auto variable = m_variables->get<FormatStringVariable>(i);
        if (!variable) {
            continue;
        }
        if (!variable->validGet()) {
            variablesValidSet(false);
            return;
        }
    }
    variablesValidSet(true);
}

FormatStringVariable::FormatStringVariable(FormatStringSplitter *parent, int index)
    : QObject(parent)
    , m_index(index)
{
    connect(parent, &FormatStringSplitter::allowedPropertyNamesChanged, this, [this]() {
        validSet(splitter()->validate(nameGet()));
    });
    connect(this, &FormatStringVariable::nameChanged, this, &FormatStringVariable::onNameChanged);
}

FormatStringSplitter *FormatStringVariable::splitter() {
    return qobject_cast<FormatStringSplitter*>(parent());
}

void FormatStringVariable::onNameChanged() {
    validSet(splitter()->validate(nameGet()));
}
