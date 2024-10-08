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
#include "protocol.h"
#include "messagelistfilter.h"
#include "lithcore_export.h"

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

class LITHCORE_EXPORT Nick : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    PROPERTY(char, visible)
    PROPERTY(char, group)
    PROPERTY(int, level)
    PROPERTY(FormattedString, name)
    PROPERTY(FormattedString, color)
    PROPERTY(FormattedString, prefix)
    PROPERTY(FormattedString, prefix_color)

    PROPERTY(weechat_pointer_t, ptr)

    Q_PROPERTY(QString colorlessName READ colorlessName NOTIFY nameChanged)
public:
    explicit Nick(Buffer* parent = nullptr);

    QString colorlessName() const;
};

class LITHCORE_EXPORT Buffer : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_DISABLE_COPY_MOVE(Buffer)
    Q_PROPERTY(weechat_pointer_t ptr READ ptr NOTIFY ptrChanged)
    PROPERTY(int, number, 0)
    PROPERTY(FormattedString, name)
    PROPERTY(FormattedString, short_name)
    ALIAS(FormattedString, name, full_name)
    Q_PROPERTY(FormattedString title READ titleGet WRITE titleSet NOTIFY titleChanged)
    PROPERTY(StringMap, local_variables)
    Q_PROPERTY(QStringList local_variables_stringList READ local_variables_stringListGet NOTIFY local_variablesChanged)

    PROPERTY(int, unreadMessages)
    PROPERTY(int, hotMessages)
    Q_PROPERTY(int totalUnreadMessages READ totalUnreadMessagesGet NOTIFY totalUnreadMessagesChanged)

    Q_PROPERTY(MessageFilterList* lines_filtered READ lines_filtered CONSTANT)
    Q_PROPERTY(QmlObjectList* lines READ lines CONSTANT)
    Q_PROPERTY(QmlObjectList* nicks READ nicks CONSTANT)
    Q_PROPERTY(int normals READ normalsGet NOTIFY nicksChanged)
    Q_PROPERTY(int voices READ voicesGet NOTIFY nicksChanged)
    Q_PROPERTY(int ops READ opsGet NOTIFY nicksChanged)
    Q_PROPERTY(QString server READ serverGet NOTIFY local_variablesChanged)
    Q_PROPERTY(bool isServer READ isServerGet NOTIFY local_variablesChanged)
    Q_PROPERTY(bool isChannel READ isChannelGet NOTIFY local_variablesChanged)
    Q_PROPERTY(bool isPrivate READ isPrivateGet NOTIFY local_variablesChanged)
    Q_PROPERTY(bool isRelay READ isRelayGet NOTIFY local_variablesChanged)

    PROPERTY(QString, lastUserInput, QStringLiteral(""))
public:
    Buffer(Lith* parent, weechat_pointer_t pointer);
    ~Buffer() override;

    Lith* lith();

    // BufferLine *getLine(weechat_pointer_t ptr);
    void prependLine(BufferLine* line);
    void appendLine(BufferLine* line);

    const FormattedString& titleGet() const;
    void titleSet(const FormattedString& o);

    bool isAfterInitialFetch() const;

    weechat_pointer_t ptr() const;
    QmlObjectList* lines();
    QmlObjectList* nicks();
    MessageFilterList* lines_filtered();
    Q_INVOKABLE Nick* getNick(weechat_pointer_t ptr);
    void addNick(weechat_pointer_t ptr, Nick* nick);
    void removeNick(weechat_pointer_t ptr);
    void clearNicks();
    Q_INVOKABLE QStringList getVisibleNicks();
    int normalsGet() const;
    int voicesGet() const;
    int opsGet() const;

    QStringList local_variables_stringListGet() const;
    QString serverGet() const;
    bool isServerGet() const;
    bool isChannelGet() const;
    bool isPrivateGet() const;
    bool isRelayGet() const;

    int totalUnreadMessagesGet() const;

signals:
    void nicksChanged();
    void titleChanged();
    void totalUnreadMessagesChanged();
    void ptrChanged();

public slots:
    bool userInput(const QString& data);
    bool rawInput(const QByteArray& data) const;
    void fetchMoreLines();
    void clearHotlist();

private:
    QmlObjectList* m_lines {nullptr};
    QmlObjectList* m_nicks {nullptr};
    MessageFilterList* m_proxyLinesFiltered {nullptr};
    weechat_pointer_t m_ptr = 0;
    bool m_afterInitialFetch {false};
    int m_lastRequestedCount {0};
    FormattedString m_title {};
};

class LITHCORE_EXPORT BufferLine : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    PROPERTY(weechat_pointer_t, ptr)
    PROPERTY(QDateTime, date)
    PROPERTY(bool, displayed)
    PROPERTY(bool, highlight)
    PROPERTY(QStringList, tags_array)

    Q_PROPERTY(QString dateString READ dateString NOTIFY dateChanged)
    Q_PROPERTY(QString nick READ nickGet NOTIFY prefixChanged)
    Q_PROPERTY(FormattedString prefix READ prefixGet WRITE prefixSet NOTIFY prefixChanged)
    Q_PROPERTY(FormattedString message READ messageGet WRITE messageSet NOTIFY messageChanged)
    Q_PROPERTY(QString dateAndPrefix READ dateAndPrefixGet NOTIFY dateAndPrefixChanged)

    Q_PROPERTY(bool isJoinPartQuitMsg READ isJoinPartQuitMsgGet NOTIFY tags_arrayChanged)
    Q_PROPERTY(bool isPrivMsg READ isPrivMsgGet NOTIFY tags_arrayChanged)
    Q_PROPERTY(bool isSelfMsg READ isSelfMsgGet NOTIFY tags_arrayChanged)
    Q_PROPERTY(QColor nickColor READ nickColorGet NOTIFY messageChanged)
    Q_PROPERTY(QString colorlessNickname READ colorlessNicknameGet NOTIFY messageChanged)
    Q_PROPERTY(QString colorlessText READ colorlessTextGet NOTIFY messageChanged)  // used here because segments is already chopped up
    Q_PROPERTY(QObject* buffer READ bufferGet CONSTANT)
public:
    explicit BufferLine(Buffer* parent);

    Buffer* buffer();
    Lith* lith();

    void setParent(Buffer* parent);

    QString dateString() const;
    const FormattedString& prefixGet() const;
    void prefixSet(const FormattedString& o);
    QString nickGet() const;
    const FormattedString& messageGet() const;
    void messageSet(const FormattedString& o);
    QString dateAndPrefixGet() const;

    bool isJoinPartQuitMsgGet();
    bool isPrivMsgGet();
    bool isSelfMsgGet();
    QColor nickColorGet() const;
    QString colorlessNicknameGet();
    QString colorlessTextGet();

    QObject* bufferGet();

    Q_INVOKABLE bool searchCompare(const QString& term);

signals:
    void messageChanged();
    void prefixChanged();
    void dateAndPrefixChanged();

private slots:

private:
    FormattedString m_message;
    FormattedString m_prefix;
    QString m_nick;
};

class LITHCORE_EXPORT HotListItem : public QObject {
    Q_OBJECT
    PROPERTY(QList<int>, count)
    Q_PROPERTY(Buffer* buffer READ bufferGet WRITE bufferSet NOTIFY bufferChanged)
public:
    explicit HotListItem(QObject* parent = nullptr);

    Buffer* bufferGet();
    void bufferSet(Buffer* o);

signals:
    void bufferChanged();

private slots:
    void onCountChanged();

private:
    QPointer<Buffer> m_buffer;
};
#endif  // DATAMODEL_H
