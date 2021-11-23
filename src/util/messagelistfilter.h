#ifndef MESSAGELISTFILTER_H
#define MESSAGELISTFILTER_H

#include "common.h"

#include <QSortFilterProxyModel>

class MessageFilterList : public QSortFilterProxyModel {
    Q_OBJECT
    PROPERTY(QString, filterWord)
public:
    MessageFilterList(QObject *parent = nullptr, QAbstractListModel *parentModel = nullptr);

    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // MESSAGELISTFILTER_H
