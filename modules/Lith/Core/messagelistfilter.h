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
#include "lithcore_export.h"

#include <QSortFilterProxyModel>

class LITHCORE_EXPORT MessageFilterList : public QSortFilterProxyModel {
    Q_OBJECT
    PROPERTY(QString, filterWord)
public:
    explicit MessageFilterList(QObject* parent = nullptr, QAbstractListModel* parentModel = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    // Mimic QmlObjectList here
    Q_INVOKABLE int count();
    Q_INVOKABLE QVariant at(const int& i);
};

#endif  // MESSAGELISTFILTER_H
