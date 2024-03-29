#ifndef QMLOBJECTLIST_H
#define QMLOBJECTLIST_H

#include <QMetaObject>
#include <QAbstractListModel>
#include <QSharedPointer>

#include "lithcore_export.h"

using QObjectPointer = QSharedPointer<QObject>;

class LITHCORE_EXPORT QmlObjectList : public QAbstractListModel {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(QmlObjectList)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    ~QmlObjectList() override {
        mData.clear();
    }

    template <typename T> inline static QmlObjectList* create(QObject* parent = Q_NULLPTR, bool deleteChildren = true) {
        return new QmlObjectList(&T::staticMetaObject, parent, deleteChildren);
    }

    void prepend(QObject* object);
    void append(QObject* object);

    bool insert(const int& i, QObject* object);

    Q_INVOKABLE int count();

    void clear();

    Q_INVOKABLE
    /**
     * @brief append
     * append an object to list and assign given properties.
     * @param properties
     * @warning if append new Object from QML is needed
     * have to assign the default value for the parameters,
     * and adding Q_INVOKABLE to constructor.
     */
    void append(const QVariantMap& properties);

    Q_INVOKABLE bool removeRow(int row, const QModelIndex& parent = QModelIndex());

    Q_INVOKABLE bool removeItem(QObject* item);

    Q_INVOKABLE inline void removeFirst() {
        if (!mData.isEmpty()) {
            removeRow(0);
        }
    }

    Q_INVOKABLE inline void removeLast() {
        if (!mData.isEmpty()) {
            removeRow(rowCount() - 1);
        }
    }

    Q_INVOKABLE QVariant at(const int& i);
    Q_INVOKABLE inline QVariant first() {
        return at(0);
    }
    Q_INVOKABLE inline QVariant last() {
        return at(count() - 1);
    }

    template <typename T> inline T* get(int i) {
        return qobject_cast<T*>(mData.at(i).data());
    }
    template <typename T> inline T* getLast() {
        return qobject_cast<T*>(last().data());
    }
    template <typename T> inline T* getFirst() {
        return qobject_cast<T*>(first().data());
    }

protected:
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

signals:
    void countChanged();

private:
    explicit QmlObjectList(const QMetaObject* m, QObject* parent = Q_NULLPTR, bool deleteChildren = true);

    const QMetaObject* mMetaObject;
    QList<QObjectPointer> mData;
    bool mDeleteChildren = true;
};

#endif  // QMLOBJECTLIST_H
