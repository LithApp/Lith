// Lith
// Copyright (C) 2020 Martin Bříza
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <http://www.gnu.org/licenses/>.

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
    if (!sourceModel()) {
        return false;
    }
    auto index = sourceModel()->index(source_row, 0, source_parent);
    auto v = sourceModel()->data(index);
    auto *n = qvariant_cast<Nick *>(v);
    if (n) {
        return n->visibleGet() &&
               n->levelGet() == 0 &&
               n->nameGet().toLower().contains(filterWordGet().toLower());
    }
    return false;
}
