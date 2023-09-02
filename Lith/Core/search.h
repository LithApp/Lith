#ifndef SEARCH_H
#define SEARCH_H

#include "common.h"
#include "datamodel.h"
#include <QPointer>

class Lith;
class Buffer;
class BufferLine;
class Search : public QObject {
    Q_OBJECT
    PROPERTY(QString, term, "")
    PROPERTY(QList<BufferLine*>, matches)
    PROPERTY_READONLY_PRIVATESETTER(int, highlightedMatchIndex, -1)
    Q_PROPERTY(BufferLine* highlightedLine READ highlightedLine NOTIFY highlightedMatchIndexChanged)
    Q_PROPERTY(bool nextEnabled READ nextEnabled NOTIFY nextEnabledChanged)
    Q_PROPERTY(bool previousEnabled READ previousEnabled NOTIFY previousEnabledChanged)
public:
    explicit Search(Lith* parent = nullptr);

    Lith* lith();
    BufferLine* highlightedLine();
    bool nextEnabled() const;
    bool previousEnabled() const;

public slots:
    void resetHighlight();
    void highlightNext();
    void highlightPrevious();

private slots:
    void invalidate();
    void onMessagesAdded(const QModelIndex& parent, int first, int last);

signals:
    void nextEnabledChanged();
    void previousEnabledChanged();

private:
    QPointer<Buffer> m_previousBuffer;
    QPointer<Buffer> m_currentBuffer;
};

#endif  // SEARCH_H
