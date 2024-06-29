#include "search.h"
#include "lith.h"

Search::Search(Lith* parent)
    : QObject {parent} {
    connect(parent, &Lith::selectedBufferChanged, this, &Search::invalidate);
    connect(this, &Search::termChanged, this, &Search::invalidate);
    connect(parent, &Lith::selectedBufferChanged, this, [this]() {
        if (m_currentBuffer) {
            disconnect(m_currentBuffer->lines(), &QmlObjectList::rowsInserted, this, &Search::onMessagesAdded);
            disconnect(m_currentBuffer->lines(), &QmlObjectList::rowsRemoved, this, &Search::invalidate);
        }
        m_previousBuffer = m_currentBuffer;
        m_currentBuffer = lith()->selectedBuffer();
        if (m_currentBuffer) {
            connect(m_currentBuffer->lines(), &QmlObjectList::rowsInserted, this, &Search::onMessagesAdded);
            connect(m_currentBuffer->lines(), &QmlObjectList::rowsRemoved, this, &Search::invalidate);
        }
    });
    connect(this, &Search::matchesChanged, this, &Search::nextEnabledChanged);
    connect(this, &Search::matchesChanged, this, &Search::previousEnabledChanged);
    connect(this, &Search::highlightedMatchIndexChanged, this, &Search::nextEnabledChanged);
    connect(this, &Search::highlightedMatchIndexChanged, this, &Search::previousEnabledChanged);
}

Lith* Search::lith() {
    return qobject_cast<Lith*>(parent());
}

BufferLine* Search::highlightedLine() {
    if (m_term.isEmpty()) {
        return nullptr;
    }
    if (m_highlightedMatchIndex < 0 || m_highlightedMatchIndex >= m_matches.count()) {
        return nullptr;
    }
    return m_matches[m_highlightedMatchIndex];
}

bool Search::nextEnabled() const {
    return m_highlightedMatchIndex + 1 < m_matches.size();
}

bool Search::previousEnabled() const {
    return m_highlightedMatchIndex >= 0;
}

void Search::resetHighlight() {
    highlightedMatchIndexSet(-1);
}

void Search::highlightNext() {
    if (m_highlightedMatchIndex + 1 < m_matches.count()) {
        highlightedMatchIndexSet(highlightedMatchIndexGet() + 1);
    }
}

void Search::highlightPrevious() {
    if (m_highlightedMatchIndex >= 0 && m_highlightedMatchIndex < m_matches.count()) {
        highlightedMatchIndexSet(highlightedMatchIndexGet() - 1);
    }
}

void Search::invalidate() {
    highlightedMatchIndexSet(-1);
    if (termGet().isEmpty()) {
        matchesSet({});
        return;
    } else {
        if (!lith()->selectedBuffer()) {
            matchesSet({});
            return;
        } else {
            QList<BufferLine*> lines;
            auto* buf = lith()->selectedBuffer();
            for (int i = 0; i < buf->lines()->count(); i++) {
                auto* line = buf->lines()->get<BufferLine>(i);
                if (line->searchCompare(termGet())) {
                    lines.append(line);
                }
            }
            matchesSet(lines);
            return;
        }
    }
}

void Search::onMessagesAdded(const QModelIndex& parent, int first, int last) {
    Q_UNUSED(parent)
    if (termGet().isEmpty()) {
        matchesSet({});
        return;
    }
    if (!lith()->selectedBuffer()) {
        Lith::instance()->log(Logger::Unexpected, QStringLiteral("Search::onMessagesAdded was called with no buffer selected"));
        matchesSet({});
        return;
    }
    if (first != last) {
        Lith::instance()->log(
            Logger::Unexpected, QStringLiteral("Search::onMessagesAdded was called with multiple lines, this shouldn't happen")
        );
    }

    auto* buf = lith()->selectedBuffer();
    auto* line = buf->lines()->get<BufferLine>(first);
    if (line->searchCompare(termGet())) {
        auto result = matchesGet();
        if (first == 0) {
            result.prepend(line);
            if (highlightedMatchIndexGet() >= 0) {
                highlightedMatchIndexSet(highlightedMatchIndexGet() + 1);
            }
        } else {
            result.append(line);
        }
        matchesSet(result);
        return;
    }
}
