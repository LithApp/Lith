#include "nicklistfilter.h"
#include "datamodel.h"

NickListFilter::NickListFilter(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(nullptr);
    setFilterRole(Qt::UserRole);
    connect(this, &NickListFilter::filterWordChanged, [this] {
        setFilterFixedString(filterWordGet());
    });
}

bool NickListFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    if (!sourceModel())
        return false;
    auto index = sourceModel()->index(source_row, 0, source_parent);
    auto v = sourceModel()->data(index);
    auto n = qvariant_cast<Nick*>(v);
    if (n) {
        return n->visibleGet() &&
               n->levelGet() == 0 &&
               n->nameGet().toLower().contains(filterWordGet());
    }
    return false;
}
