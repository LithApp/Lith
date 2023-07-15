#ifndef REPLAYPROXY_H
#define REPLAYPROXY_H

#include "common.h"
#include <QDateTime>
#include <QFile>
#include <QTimer>

class Weechat;

class ReplayRecordingInfo : public QObject {
    Q_OBJECT
    PROPERTY_CONSTANT(int, version)
    PROPERTY_CONSTANT(QDateTime, createdAt)
    PROPERTY_CONSTANT(int, number, -1)
    PROPERTY_CONSTANT(qreal, size, -1)
    PROPERTY_CONSTANT(QString, absolutePath)
public:
    ReplayRecordingInfo(int version, QDateTime createdAt, int number, size_t size, const QString &absolutePath, QObject* parent = nullptr)
        : QObject(parent)
        , m_version(version)
        , m_createdAt(createdAt)
        , m_number(number)
        , m_size(size)
        , m_absolutePath(absolutePath)
    {

    }
    Q_INVOKABLE QString erase();
    Q_INVOKABLE QString store();
};

class BaseNetworkProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentReplayVersion READ currentReplayVersionGet CONSTANT)
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)
    Q_PROPERTY(bool replaying READ replaying NOTIFY replayingChanged)
    PROPERTY_READONLY(QList<ReplayRecordingInfo*>, existingRecordings)
public:
    inline static const int currentReplayVersion = 1;
    inline static const int maxLogFiles = 10;
    inline static const QString logFileNameTemplate = "LithReplay%1.dat";
    static QString getLogDirPath();

    enum Mode {
        Record,
        Replay,
        Disabled
    };
    Q_ENUM(Mode)

    static BaseNetworkProxy* create(QObject *parent);

    Mode mode() const;

    int currentReplayVersionGet() const { return currentReplayVersion; }
    virtual bool recording() const { return false; }
    virtual bool replaying() const { return false; }

public slots:
    virtual void onDataReceived(const QByteArray &data);

signals:
    void dataReceived(const QByteArray &data);
    void recordingChanged();
    void replayingChanged();

protected:
    explicit BaseNetworkProxy(QObject *parent = nullptr, Mode mode = Disabled);
    void printHelpAndQuitApp();

private:
    Mode m_mode;
};

class ReplayProxy : public BaseNetworkProxy {
    Q_OBJECT
    Q_PROPERTY(int totalEvents READ totalEventsGet NOTIFY loadingFinished)
    Q_PROPERTY(int currentEvent READ currentEventGet NOTIFY currentEventChanged)
public:
    friend class BaseNetworkProxy;

    virtual bool replaying() const override { return m_playing; }
    int totalEventsGet() const { return m_events.size(); }
    int currentEventGet() const { return m_currentEvent; }

public slots:
    virtual void onDataReceived(const QByteArray &data) override;

private slots:
    void readAll();
    void replayStep();

signals:
    void loadingFinished();
    void replayFinished();
    void totalEventsChanged();
    void currentEventChanged();

private:
    explicit ReplayProxy(QObject *parent = nullptr);
    QTimer m_replayTimer;
    bool m_playing = false;
    int m_replayVersion = 0;
    QDateTime m_creationTime;
    QList<QPair<QDateTime, QByteArray>> m_events;
    int m_currentEvent = 0;
    QFile m_logFile;
    QDataStream m_logDataStream;
};

class RecordProxy : public BaseNetworkProxy {
    Q_OBJECT
    PROPERTY(int, slot, -1)
public:
    friend class BaseNetworkProxy;

    virtual bool recording() const override;

public slots:
    virtual void onDataReceived(const QByteArray &data) override;

private:
    explicit RecordProxy(QObject *parent = nullptr);
    bool m_firstEvent = true;
    QDateTime m_creationTime;
    QList<QPair<QDateTime, QByteArray>> m_events;
    QFile m_logFile;
    QDataStream m_logDataStream;
};



#endif // REPLAYPROXY_H
