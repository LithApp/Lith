#ifndef NICKLISTFILTER_H
#define NICKLISTFILTER_H

#include "common.h"

#include <QSortFilterProxyModel>

class NickListFilter : public QSortFilterProxyModel {
    Q_OBJECT
    PROPERTY(QString, filterWord)
public:
    NickListFilter(QObject *parent = nullptr);

    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};


#endif // NICKLISTFILTER_H
