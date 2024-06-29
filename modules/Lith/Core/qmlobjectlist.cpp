#include "qmlobjectlist.h"
#include <QMetaProperty>
#include <QQmlEngine>
#include <QDebug>

#include "lith.h"

#define ValidateIndex(m_i) ((m_i) < 0 || (m_i) >= rowCount())

static void nullDeleter(QObject* obj) {
}

QObjectPointer makeShared(QObject* object, bool shouldDelete) {
    if (shouldDelete) {
        return QObjectPointer(object);
    }
    return QObjectPointer(object, nullDeleter);
}

void QmlObjectList::append(const QVariantMap& properties) {
    auto* newObj = mMetaObject->newInstance();
    if (newObj == Q_NULLPTR) {
        Lith::instance()->log(
            Logger::Unexpected,
            QStringLiteral("Object model cannot instantiate %1, missing constructor").arg(QString::fromLatin1(mMetaObject->className()))
        );
        return;
    }
    for (auto [key, value] : properties.asKeyValueRange()) {
        if (!newObj->setProperty(key.toUtf8().data(), value)) {
            Lith::instance()->log(
                Logger::Unexpected, QStringLiteral("Object model cannot append object of type %1 with property %2")
                                        .arg(QString::fromLatin1(mMetaObject->className()))
                                        .arg(key)
            );
        }
    }
    append(newObj);
}

void QmlObjectList::prepend(QObject* object) {
    Q_ASSERT(object->metaObject() == mMetaObject);
    beginInsertRows(QModelIndex(), 0, 0);
    mData.push_front(makeShared(object, mDeleteChildren));
    endInsertRows();
    emit countChanged();
}

void QmlObjectList::append(QObject* object) {
    insert(rowCount(), object);
}

bool QmlObjectList::insert(const int& i, QObject* object) {
    Q_ASSERT(object->metaObject() == mMetaObject);
    if (i < 0 || i > rowCount()) {
        return false;
    }
    beginInsertRows(QModelIndex(), i, i);
    mData.insert(std::next(mData.begin(), i), makeShared(object, mDeleteChildren));
    endInsertRows();
    emit countChanged();
    return true;
}

int QmlObjectList::count() {
    return rowCount();
}

bool QmlObjectList::removeRow(std::list<QObjectPointer>::iterator position, int index, const QModelIndex& parent) {
    beginRemoveRows(parent, index, index);
    mData.erase(position);
    endRemoveRows();
    emit countChanged();
    return true;
}

bool QmlObjectList::removeRow(int row, const QModelIndex& parent) {
    const int first = row;
    const int last = row;
    if (ValidateIndex(first) || ValidateIndex(last)) {
        return false;
    }
    return removeRow(std::next(mData.begin(), row), row, parent);
}

bool QmlObjectList::removeItem(QObject* item) {
    int index = 0;
    for (auto it = mData.begin(); it != mData.end(); ++it) {
        if (*it == item) {
            return removeRow(it, index);
        }
        index++;
    }
    return false;
}

QVariant QmlObjectList::at(const int& i) {
    if (i < 0 || i >= mData.size()) {
        Lith::instance()->log(
            Logger::Unexpected, QStringLiteral("Attempted to access index %1 in object model of size %1").arg(i).arg(mData.size())
        );
        return QVariant();
    }
    return QVariant::fromValue(std::next(mData.begin(), i)->data());
}

QVariant QmlObjectList::data(const QModelIndex& index, int role) const {
    Q_UNUSED(role);
    if (ValidateIndex(index.row())) {
        return QVariant();
    }
    const auto& data = *std::next(mData.begin(), index.row());
    if (data.isNull()) {
        Lith::instance()->log(
            Logger::Unexpected, QStringLiteral("Object model data accessor for type %1 at row %2 contains null data")
                                    .arg(QString::fromLatin1(mMetaObject->className()))
                                    .arg(index.row())
        );
        return QVariant();
    }
    return QVariant::fromValue(data.data());
}

QHash<int, QByteArray> QmlObjectList::roleNames() const {
    return {
        {Qt::UserRole, "modelData"}
    };
}

int QmlObjectList::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return static_cast<int>(mData.size());
}

QmlObjectList::QmlObjectList(const QMetaObject* m, QObject* parent, bool deleteChildren)
    : QAbstractListModel(parent)
    , mMetaObject(m)
    , mDeleteChildren(deleteChildren) {
}

void QmlObjectList::clear() {
    beginResetModel();
    mData.clear();
    endResetModel();
    emit countChanged();
}
