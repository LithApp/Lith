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
#include "lithcore_export.h"

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QDateTime>

class LITHCORE_EXPORT Logger : public QAbstractTableModel {
    Q_OBJECT
public:
    enum EventType {
        Unexpected,
        LineAdded,
        Network,
        Protocol,
        FormatSplitter,
        Platform
    };
    Q_ENUM(EventType)
    struct Event {
        EventType type;
        Event(EventType type, QString summary)
            : type(type)
            , summary(std::move(summary)) {
        }
        Event(EventType type, QString context, QString summary, QString details)
            : type(type)
            , context(std::move(context))
            , summary(std::move(summary))
            , details(std::move(details)) {
        }

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

    explicit Logger(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void log(Event event);

    const QList<QPair<QDateTime, Event>>& events() const;

signals:
    void eventAdded(const QDateTime& dateTime, const Event& event);

private:
    QList<QPair<QDateTime, Event>> m_events;
};

class LITHCORE_EXPORT FilteredLogger : public QSortFilterProxyModel {
    Q_OBJECT
    PROPERTY(QString, contextFilter)
    PROPERTY(bool, showLineAdded, false)
    PROPERTY(bool, showProtocol, true)
    PROPERTY(bool, showDetails, false)
public:
    explicit FilteredLogger(QObject* parent = nullptr);
    void setSourceModel(QAbstractItemModel* model) override;
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
    const Logger* logger() const;

private:
    Logger* m_logger {nullptr};
};

// So the compiler doesn't complain about enum arithmetics...
inline int operator+(Qt::ItemDataRole l, Logger::Columns r) {
    return static_cast<int>(l) + static_cast<int>(r);
}

#endif  // LOGGER_H
