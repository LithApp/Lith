#include "logger.h"
#include "lith.h"
#include <QMetaEnum>

Logger::Logger(QObject* parent)
    : QAbstractTableModel(parent) {
}

QVariant Logger::data(const QModelIndex& index, int role) const {
    if (index.isValid() && index.row() >= 0 && index.row() < m_events.count()) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case Details:
                    return m_events[index.row()].second.details;
                case DateTime:
                    return m_events[index.row()].first.time().toString();
                case Context:
                    return m_events[index.row()].second.context;
                case Summary:
                    return m_events[index.row()].second.summary;
                case Type: {
                    QMetaEnum me = QMetaEnum::fromType<Logger::EventType>();
                    return QString::fromUtf8(me.valueToKey(m_events[index.row()].second.type)).toUpper();
                }
            }
        } else {
            switch (role - Qt::UserRole) {
                case Details:
                    return m_events[index.row()].second.details;
                case DateTime:
                    return m_events[index.row()].first.time().toString();
                case Context:
                    return m_events[index.row()].second.context;
                case Summary:
                    return m_events[index.row()].second.summary;
                case Type: {
                    QMetaEnum me = QMetaEnum::fromType<Logger::EventType>();
                    return QString::fromUtf8(me.valueToKey(m_events[index.row()].second.type)).toUpper();
                }
            }
        }
    }
    return QVariant();
}

int Logger::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return static_cast<int>(m_events.count());
}

int Logger::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return _LastColumn;
}

QHash<int, QByteArray> Logger::roleNames() const {
    return {
        {        Qt::DisplayRole,  QByteArrayLiteral("display")},
        {Qt::UserRole + DateTime, QByteArrayLiteral("dateTime")},
        {    Qt::UserRole + Type,     QByteArrayLiteral("type")},
        { Qt::UserRole + Context,  QByteArrayLiteral("context")},
        { Qt::UserRole + Summary,  QByteArrayLiteral("summary")},
        { Qt::UserRole + Details,  QByteArrayLiteral("details")}
    };
}

void Logger::log(Event event) {
    if (Lith::settingsGet()->enableLoggingGet()) {
        beginInsertRows(QModelIndex(), static_cast<int>(m_events.count()), static_cast<int>(m_events.count()));
        event.summary = event.summary.trimmed();
        event.context = event.context.trimmed();
        event.details = event.details.trimmed();
        m_events.append({QDateTime::currentDateTime(), event});
        endInsertRows();
        emit eventAdded(m_events.last().first, m_events.last().second);
    }
#if LITH_IS_DEBUG_BUILD
    switch (event.type) {
        case Unexpected:
            qCritical().noquote() << event.type << event.context << event.summary << event.details;
            break;
        case Network:  // fallthrough
        case Platform:
            qWarning().noquote() << event.type << event.context << event.summary << event.details;
            break;
        case LineAdded:  // don't log
            break;
        default:
            qDebug().noquote() << event.type << event.context << event.summary << event.details;
            break;
    }
#endif  // LITH_IS_DEBUG_BUILD
}

const QList<QPair<QDateTime, Logger::Event> >& Logger::events() const {
    return m_events;
}

FilteredLogger::FilteredLogger(QObject* parent)
    : QSortFilterProxyModel(parent) {
    connect(this, &FilteredLogger::contextFilterChanged, this, [this]() { invalidateRowsFilter(); });
    connect(this, &FilteredLogger::showLineAddedChanged, this, [this]() { invalidateRowsFilter(); });
    connect(this, &FilteredLogger::showProtocolChanged, this, [this]() { invalidateRowsFilter(); });
    connect(this, &FilteredLogger::showDetailsChanged, this, [this]() { invalidateColumnsFilter(); });
}

void FilteredLogger::setSourceModel(QAbstractItemModel* model) {
    m_logger = qobject_cast<Logger*>(model);
    QSortFilterProxyModel::setSourceModel(model);
}

const Logger* FilteredLogger::logger() const {
    return m_logger;
}

bool FilteredLogger::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    Q_UNUSED(source_parent)
    if (!logger()) {
        return false;
    }
    if (source_row >= logger()->events().size()) {
        return false;
    }
    const auto& event = logger()->events().at(source_row);
    if (event.second.type == Logger::LineAdded) {
        if (!showLineAddedGet()) {
            return false;
        }
    }
    if (event.second.type == Logger::Protocol) {
        if (!showProtocolGet()) {
            return false;
        }
    }
    if (m_contextFilter.isEmpty()) {
        return true;
    }
    return logger()->events().at(source_row).second.context.contains(m_contextFilter, Qt::CaseInsensitive);
}

bool FilteredLogger::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const {
    Q_UNUSED(source_parent)
    if (source_column == Logger::Details && !m_showDetails) {
        return false;
    }
    return true;
}
