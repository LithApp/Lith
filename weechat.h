#ifndef WEECHAT_H
#define WEECHAT_H

#include <QObject>
#include <QSslSocket>

#include <QDataStream>
#include <QMap>

#include <QQmlListProperty>
#include <QtQml>

#include <QSettings>
#include <QClipboard>

class Buffer;
class BufferLine;
class LineModel;

typedef uint64_t pointer_t;

#define PROPERTY_NOSETTER(type, name) \
    private: \
        Q_PROPERTY(type name READ name ## Get WRITE name ## Set NOTIFY name ## Changed) \
        type m_ ## name { }; \
    public: \
        type name ## Get () const { return m_ ## name; } \
        Q_SIGNAL void name ## Changed();

#define PROPERTY(type, name) \
    PROPERTY_NOSETTER(type, name) \
    public: \
        void name ## Set (const type &o) { \
            if (m_ ## name != o) { \
                m_ ## name = o; \
                emit name ## Changed(); \
            } \
        }

#define ALIAS(type, orig, alias) \
    Q_PROPERTY(type alias READ orig ## Get WRITE orig ## Set NOTIFY orig ## Changed)


class Weechat : public QObject
{
    Q_OBJECT
public:
    enum Status {
        UNCONFIGURED,
        CONNECTING,
        CONNECTED,
        DISCONNECTED,
        ERROR
    }; Q_ENUMS(Status)
    PROPERTY(Status, status)
    PROPERTY(QString, errorString)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY settingsChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY settingsChanged)
    Q_PROPERTY(bool encrypted READ encrypted WRITE setEncrypted NOTIFY settingsChanged)
    Q_PROPERTY(bool hasPassphrase READ hasPassphrase NOTIFY hasPassphraseChanged)

    Q_PROPERTY(int fetchFrom READ fetchFrom NOTIFY fetchFromChanged)
    Q_PROPERTY(int fetchTo READ fetchTo NOTIFY fetchToChanged)
public:
    static Weechat *_self;
    static Weechat *instance();

    int fetchFrom();
    int fetchTo();

private:
    explicit Weechat(QObject *parent = 0);

    QString host() const;
    int port() const;
    bool encrypted() const;
    bool hasPassphrase() const;

signals:
    void settingsChanged();
    void hasPassphraseChanged();

private slots:
    void onSettingsChanged();
    void requestHotlist();

public slots:
    void start();

    void setHost(const QString &value);
    void setPort(int value);
    void setEncrypted(bool value);
    void setPassphrase(const QString &value);

signals:
    void fetchFromChanged();
    void fetchToChanged();

public slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError e);
    void onSslErrors(const QList<QSslError> errors);
    void onMessageReceived(QByteArray &data);

    void input(pointer_t ptr, const QString &data);
    void fetchLines(pointer_t ptr, int count);

private:
    QSslSocket *m_connection { nullptr };

    QByteArray m_fetchBuffer;
    int32_t m_bytesRemaining { 0 };

    QString m_host { };
    int m_port { };
    QString m_passphrase { };
    bool m_useEncryption { true };

    QSettings m_settings;

    QTimer m_hotlistTimer;
    QTimer m_reconnectTimer;
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
    ALIAS(QString, name, full_name)
    PROPERTY(QString, title)
    PROPERTY(QStringList, local_variables)

    PROPERTY(int, unreadMessages)
    PROPERTY(int, hotMessages)

    Q_PROPERTY(LineModel *lines READ lines CONSTANT)
    Q_PROPERTY(QList<QObject*> nicks READ nicks NOTIFY nicksChanged)
public:
    Buffer(QObject *parent, pointer_t pointer);
    //BufferLine *getLine(pointer_t ptr);
    void appendLine(BufferLine *line);

    bool isAfterInitialFetch();

    LineModel *lines();
    QList<QObject*> nicks();
    Q_INVOKABLE Nick *getNick(pointer_t ptr);

public slots:
    void input(const QString &data);
    void fetchMoreLines();

signals:
    void nicksChanged();

private:
    LineModel *m_lines { nullptr };
    QList<QObject*> m_nicks {};
    pointer_t m_ptr;
    bool m_afterInitialFetch { false };
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
    PROPERTY(QString, summary)
public:
    BufferLineSegment(BufferLine *parent = nullptr, const QString &text = QString(), Type type = PLAIN);

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
    Q_PROPERTY(QObject *buffer READ bufferGet WRITE bufferSet NOTIFY bufferChanged)
    Q_PROPERTY(QList<QObject*> segments READ segments NOTIFY segmentsChanged)
public:
    BufferLine(QObject *parent);

    bool isPrivMsg();

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

class LineModel : public QAbstractListModel {
    Q_OBJECT
public:
    LineModel(Buffer *parent);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void appendLine(BufferLine *line);
private:
    QList<BufferLine*> m_lines;
    QSet<pointer_t> m_ptrs;
};

class HotListItem : public QObject {
    Q_OBJECT
    PROPERTY(QList<int>, count)
    PROPERTY_NOSETTER(Buffer*, buffer)
public:
    HotListItem(QObject *parent = nullptr);

    void bufferSet(Buffer *o);

private slots:
    void onCountChanged();
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
    QMap<pointer_t, HotListItem*> m_hotList;
};

class ClipboardProxy : public QObject {
    Q_OBJECT
public:
    ClipboardProxy(QObject *parent = nullptr);

    Q_INVOKABLE bool hasImage();

    Q_INVOKABLE QString text();

private:
    QClipboard *m_clipboard;
};

#endif // WEECHAT_H
