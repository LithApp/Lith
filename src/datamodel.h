// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Jakub Mach
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

#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "common.h"
#include "qmlobjectlist.h"

#include <QObject>
#include <QDateTime>
#include <QAbstractListModel>
#include <QSet>
#include <QPointer>

class Buffer;
class BufferLine;
class LineModel;
class Lith;

#include <cstdint>

class Nick : public QObject {
    Q_OBJECT
    PROPERTY(char, visible)
    PROPERTY(char, group)
    PROPERTY(int, level)
    PROPERTY(QString, name)
    PROPERTY(QString, color)
    PROPERTY(QString, prefix)
    PROPERTY(QString, prefix_color)

    PROPERTY(pointer_t, ptr)
public:
    Nick(Buffer *parent = nullptr);
    virtual ~Nick();

};

class Buffer : public QObject {
    Q_OBJECT
    PROPERTY(int, number)
    PROPERTY(QString, name)
    PROPERTY(QString, short_name)
    ALIAS(QString, name, full_name)
    Q_PROPERTY(QString title READ titleGet WRITE titleSet NOTIFY titleChanged)
    PROPERTY(StringMap, local_variables)
    Q_PROPERTY(QStringList local_variables_stringList READ local_variables_stringListGet NOTIFY local_variablesChanged)

    PROPERTY(int, unreadMessages)
    PROPERTY(int, hotMessages)

    Q_PROPERTY(QmlObjectList *lines READ lines CONSTANT)
    Q_PROPERTY(QmlObjectList *nicks READ nicks CONSTANT)
    Q_PROPERTY(int normals READ normalsGet NOTIFY nicksChanged)
    Q_PROPERTY(int voices READ voicesGet NOTIFY nicksChanged)
    Q_PROPERTY(int ops READ opsGet NOTIFY nicksChanged)
    Q_PROPERTY(bool isServer READ isServerGet NOTIFY local_variablesChanged)
    Q_PROPERTY(bool isChannel READ isChannelGet NOTIFY local_variablesChanged)
    Q_PROPERTY(bool isPrivate READ isPrivateGet NOTIFY local_variablesChanged)
public:
    Buffer(Lith *parent, pointer_t pointer);
    virtual ~Buffer();

    Lith *lith();

    //BufferLine *getLine(pointer_t ptr);
    void prependLine(BufferLine *line);
    void appendLine(BufferLine *line);

    QString titleGet() const;
    void titleSet(const QString &o);

    bool isAfterInitialFetch();

    QmlObjectList *lines();
    QmlObjectList *nicks();
    Q_INVOKABLE Nick *getNick(pointer_t ptr);
    void addNick(pointer_t ptr, Nick* nick);
    void removeNick(pointer_t ptr);
    void clearNicks();
    Q_INVOKABLE QStringList getVisibleNicks();
    int normalsGet() const;
    int voicesGet() const;
    int opsGet() const;

    QStringList local_variables_stringListGet() const;
    bool isServerGet() const;
    bool isChannelGet() const;
    bool isPrivateGet() const;

signals:
    void nicksChanged();
    void titleChanged();

public slots:
    bool input(const QString &data);
    void fetchMoreLines();
    void clearHotlist();

private:
    QmlObjectList *m_lines { nullptr };
    QmlObjectList *m_nicks { nullptr };
    pointer_t m_ptr;
    bool m_afterInitialFetch { false };
    int m_lastRequestedCount { 0 };
    QString m_title {};
};

class BufferLineSegment : public QObject {
    Q_OBJECT
public:
    enum Type {
        PLAIN,
        LINK,
        EMBED,
        IMAGE,
        VIDEO
    }; Q_ENUMS(Type)
    PROPERTY(Type, type)
    PROPERTY(QString, plainText)
    Q_PROPERTY(QString summary READ summaryGet NOTIFY summaryChanged)
    PROPERTY(QString, embedUrl)
public:
    BufferLineSegment(BufferLine *parent = nullptr, const QString &text = QString(), Type type = PLAIN);

signals:
    void summaryChanged();

private:
    QString summaryGet();
};

class BufferLine : public QObject {
    Q_OBJECT
    PROPERTY(QDateTime, date)
    PROPERTY(bool, displayed)
    PROPERTY(bool, highlight)
    PROPERTY(QStringList, tags_array)

    Q_PROPERTY(QString nickAttribute READ nickAttributeGet NOTIFY prefixChanged)
    Q_PROPERTY(QString nickAttributeColor READ nickAttributeColorGet NOTIFY prefixChanged)
    Q_PROPERTY(QString nick READ nickGet NOTIFY prefixChanged)
    Q_PROPERTY(QString nickColor READ nickColorGet NOTIFY prefixChanged)
    Q_PROPERTY(QString prefix READ prefixGet WRITE prefixSet NOTIFY prefixChanged)
    Q_PROPERTY(QString message READ messageGet WRITE messageSet NOTIFY messageChanged)

    Q_PROPERTY(bool isPrivMsg READ isPrivMsgGet NOTIFY tags_arrayChanged)
    Q_PROPERTY(bool isSelfMsg READ isSelfMsgGet NOTIFY tags_arrayChanged)
    Q_PROPERTY(QString colorlessNickname READ colorlessNicknameGet NOTIFY messageChanged)
    Q_PROPERTY(QString colorlessText READ colorlessTextGet NOTIFY messageChanged) // used here because segments is already chopped up
    Q_PROPERTY(QObject *buffer READ bufferGet CONSTANT)
    Q_PROPERTY(QList<QObject*> segments READ segments NOTIFY segmentsChanged)
public:
    BufferLine(Buffer *parent);
    virtual ~BufferLine();

    Buffer *buffer();
    Lith *lith();

    void setParent(Buffer *parent);

    QString prefixGet() const;
    void prefixSet(const QString &o);
    QString nickAttributeGet() const;
    QString nickAttributeColorGet() const;
    QString nickGet() const;
    QString nickColorGet() const;
    QString messageGet() const;
    void messageSet(const QString &o);

    bool isPrivMsgGet();
    bool isSelfMsgGet();
    QString colorlessNicknameGet();
    QString colorlessTextGet();

    QObject *bufferGet();

    QList<QObject*> segments();

signals:
    void segmentsChanged();
    void messageChanged();
    void prefixChanged();

private slots:
    void onMessageChanged();

private:
    QString m_message;
    QString m_nickAttr;
    QString m_nick;
    QString m_nickAttrColor;
    QString m_nickColor;
    QList<QObject*> m_segments;
};

class HotListItem : public QObject {
    Q_OBJECT
    PROPERTY(QList<int>, count)
    Q_PROPERTY(Buffer* buffer READ bufferGet WRITE bufferSet NOTIFY bufferChanged)
public:
    HotListItem(QObject *parent = nullptr);

    Buffer *bufferGet();
    void bufferSet(Buffer *o);

signals:
    void bufferChanged();

private slots:
    void onCountChanged();

private:
    QPointer<Buffer> m_buffer;
};
#endif // DATAMODEL_H
