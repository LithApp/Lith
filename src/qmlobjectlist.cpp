#include "qmlobjectlist.h"
#include <QMetaProperty>
#include <QQmlEngine>
#include <QDebug>

#include "lith.h"

#define ValidateIndex(m_i) (m_i < 0 || m_i >= rowCount())

void QmlObjectList::append(const QVariantMap& properties)
{
    auto* newObj = mMetaObject.newInstance();
    if(newObj == Q_NULLPTR) {
        Lith::instance()->log(Logger::Unexpected, QString("Object model cannot instantiate %1, missing constructor").arg(mMetaObject.className()));
        return;
    }
    for(const QString& key : properties.keys()) {
        if(!newObj->setProperty(key.toUtf8().data(), properties.value(key))) {
            Lith::instance()->log(Logger::Unexpected, QString("Object model cannot append object of type %1 with property %2").arg(mMetaObject.className()).arg(key));
        }
    }
    append(newObj);
}

void QmlObjectList::prepend(QObject *object) {
    Q_ASSERT(object->metaObject() == &mMetaObject);
    beginInsertRows(QModelIndex(), 0, 0);
    mData.prepend(QObjectPointer(object));
    endInsertRows();
}

void QmlObjectList::append(QObject *object)
{
    insert(rowCount(), object);
}

bool QmlObjectList::insert(const int& i, QObject* object)
{
    Q_ASSERT(object->metaObject() == &mMetaObject);
    if(i < 0 || i > rowCount())
        return false;
    beginInsertRows(QModelIndex(), i, i);
    mData.insert(i, QObjectPointer(object));
    endInsertRows();
    return true;
}

int QmlObjectList::count() {
    return rowCount();
}

bool QmlObjectList::removeRow(int row, const QModelIndex &parent)
{
    const int first = row;
    const int last = row;
    if(ValidateIndex(first) || ValidateIndex(last))
        return false;
    beginRemoveRows(parent, first, last);
    mData.removeAt(row);
    endRemoveRows();
    return true;
}

bool QmlObjectList::removeItem(QObject *item) {
    for (int i = 0; i < mData.count(); i++) {
        if (mData[i] == item) {
            return removeRow(i);
        }
    }
    return false;
}

QVariant QmlObjectList::at(const int &i)
{
    if (i < 0 || i >= mData.size()) {
        Lith::instance()->log(Logger::Unexpected, QString("Attempted to access index %1 in object model of size %1").arg(i).arg(mData.size()));
        return QVariant();
    }
    return QVariant::fromValue(mData.at(i));
}

QVariant QmlObjectList::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
    if(ValidateIndex(index.row()))
        return QVariant();
    const auto& data = mData[index.row()];
    if(data.isNull()) {
        Lith::instance()->log(Logger::Unexpected, QString("Object model data accessor for type %1 at row %2 contains null data").arg(mMetaObject.className()).arg(index.row()));
        return QVariant();
    }
    return QVariant::fromValue(data.data());
}

QHash<int, QByteArray> QmlObjectList::roleNames() const
{
    return { { Qt::UserRole, "modelData" } };
}

int QmlObjectList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mData.count();
}

QmlObjectList::QmlObjectList(const QMetaObject &m, QObject *parent) :
    QAbstractListModel(parent),
    mMetaObject(m)
{ }

void QmlObjectList::clear() {
    beginResetModel();
    mData.clear();
    endResetModel();
}

