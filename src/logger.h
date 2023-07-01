// Lith
// Copyright (C) 2023 Martin Bříza
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


#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QDateTime>


class Logger : public QAbstractTableModel {
    Q_OBJECT
public:
    enum EventType {
        Unexpected,
        LineAdded,
        Network,
        Protocol
    };
    Q_ENUM(EventType)
    struct Event {
        EventType type;
        Event(EventType type, QString summary) : type(type), summary(summary) {}
        Event(EventType type, QString context, QString summary, QString details) : type(type), context(context), summary(summary), details(details) {}

        QString context;
        QString summary;
        QString details;
    };
    enum Columns {
        DateTime = 0,
        Type,
        Context,
        Summary,
        Details,
        _LastColumn,
    };
    Logger(QObject *parent = nullptr);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void log(Event event);

    const QList<QPair<QDateTime, Event>>& events() const;;

private:
    QList<QPair<QDateTime, Event>> m_events;
};

class FilteredLogger : public QSortFilterProxyModel {
    Q_OBJECT
    PROPERTY(QString, contextFilter, "")
    PROPERTY(bool, showLineAdded, false)
    PROPERTY(bool, showProtocol, true)
    PROPERTY(bool, showDetails, false)
public:
    FilteredLogger(QObject* parent = nullptr);
    void setSourceModel(QAbstractItemModel* model) override;
    const Logger *logger() const;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    virtual bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
private:
    Logger *m_logger { nullptr };
};

#endif // LOGGER_H
