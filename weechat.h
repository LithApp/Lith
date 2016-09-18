#ifndef WEECHAT_H
#define WEECHAT_H

#include <QObject>
#include <QSslSocket>

#include <QDataStream>
#include <QMap>

#include <QQmlListProperty>
#include <QtQml>

#include <QSettings>

class BufferLine;
class LineModel;

typedef uint64_t pointer_t;

class Weechat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY settingsChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY settingsChanged)
    Q_PROPERTY(bool encrypted READ encrypted WRITE setEncrypted NOTIFY settingsChanged)
public:
    static Weechat *_self;
    static Weechat *instance();
private:
    explicit Weechat(QObject *parent = 0);

    QString host() const;
    int port() const;
    bool encrypted() const;

signals:
    void settingsChanged();

private slots:
    void onSettingsChanged();

public slots:
    void start();

    void setHost(const QString &value);
    void setPort(int value);
    void setEncrypted(bool value);
    void setPassphrase(const QString &value);

signals:

public slots:
    void onReadyRead();
    void onConnected();
    void onError(QAbstractSocket::SocketError e);
    void onMessageReceived(QByteArray &data);

    void input(pointer_t ptr, const QString &data);

private:
    QSslSocket *m_connection { nullptr };

    QString m_host { };
    int m_port { };
    QString m_passphrase { };
    bool m_useEncryption { true };

    QSettings m_settings;
};


namespace W {
    struct Char { char d; };
    struct Integer { int32_t d; };
    struct LongInteger { int64_t d; };
    struct String { QString d; };
    struct Buffer { QByteArray d; };
    struct Pointer { pointer_t d; };
    struct Time { QString d; };
    struct HashTable { QMap<QString, QString> d; };
    struct HData { };
    struct ArrayInt { QList<int> d; };
    struct ArrayStr { QStringList d; };

    QDataStream &operator>>(QDataStream &s, Char &r);
    QDataStream &operator>>(QDataStream &s, Integer &r);
    QDataStream &operator>>(QDataStream &s, LongInteger &r);
    QDataStream &operator>>(QDataStream &s, String &r);
    QDataStream &operator>>(QDataStream &s, Buffer &r);
    QDataStream &operator>>(QDataStream &s, Pointer &r);
    QDataStream &operator>>(QDataStream &s, Time &r);
    QDataStream &operator>>(QDataStream &s, HashTable &r);
    QDataStream &operator>>(QDataStream &s, HData &r);
    QDataStream &operator>>(QDataStream &s, ArrayInt &r);
    QDataStream &operator>>(QDataStream &s, ArrayStr &r);
};

#define PROPERTY(type, name) \
private: \
    Q_PROPERTY(type name READ name ## Get WRITE name ## Set NOTIFY name ## Changed) \
    type m_ ## name { }; \
public: \
    type name ## Get () const { return m_ ## name; } \
    void name ## Set (const type &o) { \
        if (m_ ## name != o) { \
            m_ ## name = o; \
            emit name ## Changed(); \
        } \
    } \
    Q_SIGNAL void name ## Changed();

#define ALIAS(type, orig, alias) \
    Q_PROPERTY(type alias READ orig ## Get WRITE orig ## Set NOTIFY orig ## Changed)

class Buffer : public QObject {
    Q_OBJECT
    PROPERTY(int, number)
    PROPERTY(QString, name)
    ALIAS(QString, name, full_name)
    PROPERTY(QString, title)
    PROPERTY(QStringList, local_variables)

    Q_PROPERTY(LineModel *lines READ lines CONSTANT)
public:
    Buffer(QObject *parent, pointer_t pointer);
    //BufferLine *getLine(pointer_t ptr);
    void appendLine(BufferLine *line);

    LineModel *lines();

public slots:
    void input(const QString &data);
private:
    LineModel *m_lines { nullptr };
    pointer_t m_ptr;
};

class BufferLine : public QObject {
    Q_OBJECT
    PROPERTY(QDateTime, date)
    PROPERTY(bool, displayed)
    PROPERTY(bool, highlight)
    PROPERTY(QStringList, tags_array)
    PROPERTY(QString, prefix)
    PROPERTY(QString, message)

    Q_PROPERTY(QObject *buffer READ bufferGet WRITE bufferSet NOTIFY bufferChanged)
public:
    BufferLine(QObject *parent) : QObject(parent) { }

    QObject *bufferGet();
    void bufferSet(QObject *o);
signals:
    void bufferChanged();
};

class LineModel : public QAbstractListModel {
    Q_OBJECT
public:
    LineModel(Buffer *parent);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void appendLine(BufferLine *line);
private:
    QList<BufferLine*> m_lines;
};


class Nick : public QObject {
    Q_OBJECT
    PROPERTY(QString, name)
    PROPERTY(QString, color)
};

class StuffManager : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int bufferCount READ bufferCount NOTIFY buffersChanged)
    Q_PROPERTY(Buffer* selected READ selectedBuffer NOTIFY selectedChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedChanged)
public:
    static StuffManager *instance();
    QObject *getStuff(pointer_t ptr, const QString &type = "", pointer_t parent = 0);

    int bufferCount();
    Buffer *selectedBuffer();
    int selectedIndex();
    void setSelectedIndex(int o);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void reset();

signals:
    void buffersChanged();
    void selectedChanged();
private:
    StuffManager();
    static StuffManager *_self;

    int m_selectedIndex { 0 };

    QMap<pointer_t, Buffer*> m_bufferMap;
    QList<Buffer*> m_buffers;

    QMap<pointer_t, BufferLine*> m_lineMap;
};


#endif // WEECHAT_H
