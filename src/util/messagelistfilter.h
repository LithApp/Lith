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
