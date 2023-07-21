// Lith
// Copyright (C) 2021 Jakub Mach
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

#include "messagelistfilter.h"
#include "datamodel.h"
#include "lith.h"

MessageFilterList::MessageFilterList(QObject *parent, QAbstractListModel *parentModel)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(parentModel);
    setFilterRole(Qt::UserRole);
    connect(Lith::instance()->settingsGet(), &Settings::showJoinPartQuitMessagesChanged, this, [this]
    {
        invalidateFilter();
    });
}
bool MessageFilterList::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    if (!sourceModel())
        return true;

    auto index = sourceModel()->index(source_row, 0, source_parent);
    auto v = sourceModel()->data(index);
    auto b = qvariant_cast<BufferLine*>(v);

    if (b && !Lith::instance()->settingsGet()->showJoinPartQuitMessagesGet()) {
        return !b->isJoinPartQuitMsgGet();
    }
    else
        return b;

    return false;
}

int MessageFilterList::count() {
    return rowCount();
}

QVariant MessageFilterList::at(const int &i) {
    if (i >= 0 && i < rowCount()) {
        return data(index(i, 0), Qt::UserRole);
    }
    return QVariant();
}
