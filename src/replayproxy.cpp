#include "replayproxy.h"
#include "weechat.h"
#include "lith.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QTimer>

QString BaseNetworkProxy::getLogDirPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

BaseNetworkProxy *BaseNetworkProxy::create(QObject *parent) {
    if (qApp->arguments().contains("--replay")) {
        return new ReplayProxy(parent);
    }
    else if (Settings::instance()->enableReplayRecordingGet()) {
        return new RecordProxy(parent);
    }
    else {
        return new BaseNetworkProxy(parent);
    }
}

BaseNetworkProxy::Mode BaseNetworkProxy::mode() const {
    return m_mode;
}

void BaseNetworkProxy::onDataReceived(const QByteArray &data) {
    emit dataReceived(data);
}

BaseNetworkProxy::BaseNetworkProxy(QObject *parent, Mode mode)
    : QObject(parent)
    , m_mode(mode)
{

}

void BaseNetworkProxy::printHelpAndQuitApp() {
    qCritical() << "To replay a recording, you need to specify a recording number:";
    qCritical().noquote() << QString("\t%1 --replay [(0-9)|last]").arg(qApp->arguments().first());
    QDir dir(getLogDirPath());
    if (!dir.exists())
        dir.mkpath(".");
    auto entries = dir.entryInfoList(QStringList{logFileNameTemplate.arg("*")}, QDir::Filter::Files);
    auto entriesByTime = dir.entryInfoList(QStringList{logFileNameTemplate.arg("*")}, QDir::Filter::Files, QDir::SortFlag::Time);
    if (entries.isEmpty()) {
        qCritical() << "There are no saved recordings now. Run Lith in regular mode and enable replay recording to create some.";
    }
    else {
        qCritical() << "There are now these following recordings available:";
        QString entriesString;
        for (auto &i : entries) {
            auto justTheNumber = i.fileName();
            justTheNumber.replace("LithReplay", "");
            justTheNumber.replace(".dat", "");
            if (entriesString.isEmpty()) {
                auto justTheLatestNumber = entriesByTime.first().fileName();
                justTheLatestNumber.replace("LithReplay", "");
                justTheLatestNumber.replace(".dat", "");
                entriesString.append(QString("\tlast (%1), ").arg(justTheLatestNumber));
            }
            else {
                entriesString.append(", ");
            }
            entriesString.append(justTheNumber);
        }
        qCritical().noquote() << entriesString;
    }

    QTimer::singleShot(0, qApp, &QApplication::quit);
}


void ReplayProxy::onDataReceived(const QByteArray &data) {
    Q_UNUSED(data)
}

void ReplayProxy::readAll() {
    while (!m_logDataStream.atEnd()) {
        QPair<QDateTime, QByteArray> event;
        m_logDataStream >> event;
        if (m_logDataStream.status() == QDataStream::Ok) {
            m_events.append(event);
        }
        else {
            qCritical() << "QDataStream broke:" << m_logDataStream.status();
        }
    }


    emit loadingFinished();
    m_playing = true;
    emit replayingChanged();
}

void ReplayProxy::replayStep() {
    if (m_currentEvent < m_events.size()) {
        emit dataReceived(m_events.at(m_currentEvent).second);
        m_currentEvent++;
        emit currentEventChanged();
    }
    else {
        emit replayFinished();
        m_playing = false;
        emit replayingChanged();
    }
}

ReplayProxy::ReplayProxy(QObject *parent)
    : BaseNetworkProxy(parent, Replay)
{
    int replayArgumentIndex = qApp->arguments().indexOf("--replay");

    if ((replayArgumentIndex + 1) >= qApp->arguments().size()) {
        printHelpAndQuitApp();
        return;
    }

    QDir dir(getLogDirPath());
    QString selectedFileName;
    QString replayArgument = qApp->arguments().at(replayArgumentIndex + 1);
    if (replayArgument == "last") {
        auto entries = dir.entryInfoList(QStringList{logFileNameTemplate.arg("*")}, QDir::Filter::Files, QDir::SortFlag::Time);
        if (!entries.isEmpty())
            selectedFileName = entries.first().fileName();
    }
    else {
        bool ok = false;
        int replayNumber = replayArgument.toInt(&ok);
        if (ok) {
            selectedFileName = logFileNameTemplate.arg(replayNumber);
        }
        else {
            printHelpAndQuitApp();
            qCritical() << QString("The argument you entered was: \"%1\"").arg(replayArgument);
            return;
        }
    }

    auto fullFilePath = dir.absoluteFilePath(selectedFileName);
    m_logFile.setFileName(fullFilePath);
    if (!m_logFile.open(QIODevice::ReadOnly)) {
        printHelpAndQuitApp();
        qCritical() << QString("File \"%1\" couldn't be opened").arg(selectedFileName);
        return;
    }
    m_logDataStream.setDevice(&m_logFile);

    m_logDataStream >> m_replayVersion;
    if (m_logDataStream.status() != QDataStream::Ok || m_replayVersion != currentReplayVersion) {
        printHelpAndQuitApp();
        qCritical().noquote() << QString("File \"%1\" is an older replay version (%2), we're currently on %3").arg(selectedFileName).arg(m_replayVersion).arg(currentReplayVersion);
        return;
    }
    m_logDataStream >> m_creationTime;

    connect(this, &ReplayProxy::loadingFinished, &m_replayTimer, qOverload<>(&QTimer::start), Qt::QueuedConnection);
    connect(this, &ReplayProxy::replayFinished, &m_replayTimer, &QTimer::stop, Qt::QueuedConnection);
    connect(&m_replayTimer, &QTimer::timeout, this, &ReplayProxy::replayStep);
    m_replayTimer.setInterval(100);
    m_replayTimer.setSingleShot(false);

    QTimer::singleShot(0, this, &ReplayProxy::readAll);
}

bool RecordProxy::recording() const {
    return m_logFile.isOpen() && m_logFile.isWritable() && m_logDataStream.status() == QDataStream::Ok;
}

void RecordProxy::onDataReceived(const QByteArray &data)
{
    if (m_logFile.isOpen()) {
        QPair<QDateTime, QByteArray> newEvent{QDateTime::currentDateTime(), data};
        m_logDataStream << newEvent;
        m_logFile.flush();
        m_events.append(newEvent);
    }

    emit dataReceived(data);
}

RecordProxy::RecordProxy(QObject *parent)
    : BaseNetworkProxy(parent, Record)
    , m_creationTime(QDateTime::currentDateTime())
{
    QDir dir(getLogDirPath());
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qCritical() << QString("Logger could not create folder %1 recording will be disabled.").arg(getLogDirPath());
            return;
        }
    }
    auto entries = dir.entryInfoList(QStringList{logFileNameTemplate.arg("*")}, QDir::Filter::Files, QDir::SortFlag::Time);

    QString selectedFileName;
    // First check if all possible filenames are filled
    for (int i = 0; i < maxLogFiles; i++) {
        if (std::find_if(entries.begin(), entries.end(), [i](const auto& entry) {
                return entry.fileName() == BaseNetworkProxy::logFileNameTemplate.arg(i);
            }) == entries.end()) {
            slotSet(i);
            selectedFileName = BaseNetworkProxy::logFileNameTemplate.arg(i);
            break;
        }
    }

    // And if they are, just overwrite the oldest one
    if (selectedFileName.isEmpty()) {
        selectedFileName = entries.last().fileName();
        // Yes I realize this is an abomination and I've done it the third time now;
        auto justTheNumber = selectedFileName;
        justTheNumber.replace("LithReplay", "");
        justTheNumber.replace(".dat", "");
        slotSet(justTheNumber.toInt());
    }

    m_logFile.setFileName(dir.absoluteFilePath(selectedFileName));
    if (!m_logFile.open(QIODevice::WriteOnly)) {
        qCritical() << QString("Logger could not open %1 for writing, recording will be disabled.").arg(dir.absoluteFilePath(selectedFileName));
        return;
    }
    m_logDataStream.setDevice(&m_logFile);

    m_logDataStream << currentReplayVersion;
    m_logDataStream << m_creationTime;
}
