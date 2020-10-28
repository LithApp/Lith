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

};

class Buffer : public QObject {
    Q_OBJECT
    PROPERTY(int, number)
    PROPERTY(QString, name)
    PROPERTY(QString, short_name)
    ALIAS(QString, name, full_name)
    PROPERTY(QString, title)
    PROPERTY(QStringList, local_variables)

    PROPERTY(int, unreadMessages)
    PROPERTY(int, hotMessages)

    Q_PROPERTY(QmlObjectList *lines READ lines CONSTANT)
    Q_PROPERTY(QList<QObject*> nicks READ nicks NOTIFY nicksChanged)
public:
    Buffer(QObject *parent, pointer_t pointer);
    //BufferLine *getLine(pointer_t ptr);
    void appendLine(BufferLine *line);

    bool isAfterInitialFetch();

    QmlObjectList *lines();
    QList<QObject*> nicks();
    Q_INVOKABLE Nick *getNick(pointer_t ptr);
    Q_INVOKABLE QStringList getVisibleNicks();

public slots:
    void input(const QString &data);
    void fetchMoreLines();

signals:
    void nicksChanged();

private:
    QmlObjectList *m_lines { nullptr };
    QList<QObject*> m_nicks {};
    pointer_t m_ptr;
    bool m_afterInitialFetch { false };
    int m_lastRequestedCount { 0 };
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
    PROPERTY(QString, prefix)
    PROPERTY(QString, message)

    Q_PROPERTY(bool isPrivMsg READ isPrivMsg NOTIFY tags_arrayChanged)
    Q_PROPERTY(QString colorlessNickname READ colorlessNicknameGet)
    Q_PROPERTY(QString colorlessText READ colorlessTextGet) // used here because segments is already chopped up
    Q_PROPERTY(QObject *buffer READ bufferGet WRITE bufferSet NOTIFY bufferChanged)
    Q_PROPERTY(QList<QObject*> segments READ segments NOTIFY segmentsChanged)
public:
    BufferLine(QObject *parent);

    bool isPrivMsg();
    QString colorlessNicknameGet();
    QString colorlessTextGet();

    QObject *bufferGet();
    void bufferSet(QObject *o);

    QList<QObject*> segments();

signals:
    void bufferChanged();
    void segmentsChanged();

private slots:
    void onMessageChanged();

private:
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
