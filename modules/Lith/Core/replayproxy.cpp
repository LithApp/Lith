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

BaseNetworkProxy* BaseNetworkProxy::create(QObject* parent) {
    if (qApp->arguments().contains(QStringLiteral("--replay"))) {
        return new ReplayProxy(parent);
    } else if (Settings::instance()->enableReplayRecordingGet()) {
        return new RecordProxy(parent);
    } else {
        return new BaseNetworkProxy(parent);
    }
}

BaseNetworkProxy::Mode BaseNetworkProxy::mode() const {
    return m_mode;
}

void BaseNetworkProxy::onDataReceived(const QByteArray& data) {
    emit dataReceived(data);
}

BaseNetworkProxy::BaseNetworkProxy(QObject* parent, Mode mode)
    : QObject(parent)
    , m_mode(mode) {
    QDir dir(getLogDirPath());
    if (dir.exists()) {
        auto entriesByTime =
            dir.entryInfoList(QStringList {logFileNameTemplate.arg(QStringLiteral("*"))}, QDir::Filter::Files, QDir::SortFlag::Time);
        for (const auto& entry : entriesByTime) {
            auto justTheNumber = entry.fileName();
            justTheNumber.replace(QStringLiteral("LithReplay"), QString());
            justTheNumber.replace(QStringLiteral(".dat"), QString());
            QFile entryFile(entry.absoluteFilePath());
            if (entryFile.open(QIODevice::ReadOnly)) {
                QDataStream stream(&entryFile);
                int version = -1;
                QDateTime creationTime;
                stream >> version >> creationTime;
                if (stream.status() == QDataStream::Ok) {
                    auto* record =
                        new ReplayRecordingInfo(version, creationTime, justTheNumber.toInt(), entry.size(), entry.absoluteFilePath(), this);
                    m_existingRecordings.append(record);
                    connect(record, &QObject::destroyed, this, [this, record]() {
                        if (m_existingRecordings.contains(record)) {
                            m_existingRecordings.removeAll(record);
                            emit existingRecordingsChanged();
                        }
                    });
                } else {
                    m_existingRecordings.append(new ReplayRecordingInfo(1, QDateTime(), -1, entry.size(), entry.absoluteFilePath(), this));
                }
            } else {
                m_existingRecordings.append(new ReplayRecordingInfo(1, QDateTime(), -1, entry.size(), entry.absoluteFilePath(), this));
            }
        }
    }
}

void BaseNetworkProxy::printHelpAndQuitApp() {
    qCritical() << tr("To replay a recording, you need to specify a recording number:");
    qCritical().noquote() << QStringLiteral("\t%1 --replay [(0-9)|last]").arg(qApp->arguments().first());
    QDir dir(getLogDirPath());
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }
    auto entries = dir.entryInfoList(QStringList {logFileNameTemplate.arg(QStringLiteral("*"))}, QDir::Filter::Files);
    auto entriesByTime =
        dir.entryInfoList(QStringList {logFileNameTemplate.arg(QStringLiteral("*"))}, QDir::Filter::Files, QDir::SortFlag::Time);
    if (entries.isEmpty()) {
        qCritical() << "There are no saved recordings now. Run Lith in regular mode and enable replay recording to create some.";
    } else {
        qCritical() << "There are now these following recordings available:";
        QString entriesString;
        for (auto& i : entries) {
            auto justTheNumber = i.fileName();
            justTheNumber.replace(QStringLiteral("LithReplay"), QStringLiteral(""));
            justTheNumber.replace(QStringLiteral(".dat"), QStringLiteral(""));
            if (entriesString.isEmpty()) {
                auto justTheLatestNumber = entriesByTime.first().fileName();
                justTheLatestNumber.replace(QStringLiteral("LithReplay"), QStringLiteral(""));
                justTheLatestNumber.replace(QStringLiteral(".dat"), QStringLiteral(""));
                entriesString.append(QStringLiteral("\tlast (%1), ").arg(justTheLatestNumber));
            } else {
                entriesString.append(QStringLiteral(", "));
            }
            entriesString.append(justTheNumber);
        }
        qCritical().noquote() << entriesString;
    }

    QTimer::singleShot(0, qApp, &QApplication::quit);
}

void ReplayProxy::onDataReceived(const QByteArray& data) {
    Q_UNUSED(data)
}

void ReplayProxy::readAll() {
    while (!m_logDataStream.atEnd()) {
        QPair<QDateTime, QByteArray> event;
        m_logDataStream >> event;
        if (m_logDataStream.status() == QDataStream::Ok) {
            m_events.append(event);
        } else {
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
    } else {
        emit replayFinished();
        m_playing = false;
        emit replayingChanged();
    }
}

ReplayProxy::ReplayProxy(QObject* parent)
    : BaseNetworkProxy(parent, Replay) {
    int replayArgumentIndex = qApp->arguments().indexOf(QStringLiteral("--replay"));

    if ((replayArgumentIndex + 1) >= qApp->arguments().size()) {
        printHelpAndQuitApp();
        return;
    }

    QDir dir(getLogDirPath());
    QString selectedFileName;
    QString replayArgument = qApp->arguments().at(replayArgumentIndex + 1);
    if (replayArgument == QStringLiteral("last")) {
        auto entries =
            dir.entryInfoList(QStringList {logFileNameTemplate.arg(QLatin1Char('*'))}, QDir::Filter::Files, QDir::SortFlag::Time);
        if (!entries.isEmpty()) {
            selectedFileName = entries.first().fileName();
        }
    } else {
        bool ok = false;
        int replayNumber = replayArgument.toInt(&ok);
        if (ok) {
            selectedFileName = logFileNameTemplate.arg(replayNumber);
        } else {
            selectedFileName = replayArgument;
        }
    }

    QString fullFilePath;
    if (selectedFileName.contains(QLatin1Char('/')) || selectedFileName.contains(QLatin1Char('\\'))) {
        fullFilePath = selectedFileName;
    } else {
        fullFilePath = dir.absoluteFilePath(selectedFileName);
    }
    m_logFile.setFileName(fullFilePath);
    if (!m_logFile.open(QIODevice::ReadOnly)) {
        printHelpAndQuitApp();
        qCritical() << QStringLiteral("File \"%1\" couldn't be opened").arg(selectedFileName);
        return;
    }
    m_logDataStream.setDevice(&m_logFile);

    m_logDataStream >> m_replayVersion;
    if (m_logDataStream.status() != QDataStream::Ok || m_replayVersion != currentReplayVersion) {
        printHelpAndQuitApp();
        qCritical().noquote() << QStringLiteral("File \"%1\" is an older replay version (%2), we're currently on %3")
                                     .arg(selectedFileName)
                                     .arg(m_replayVersion)
                                     .arg(currentReplayVersion);
        return;
    }
    m_logDataStream >> m_creationTime;

    connect(this, &ReplayProxy::loadingFinished, &m_replayTimer, qOverload<>(&QTimer::start), Qt::QueuedConnection);
    connect(this, &ReplayProxy::replayFinished, &m_replayTimer, &QTimer::stop, Qt::QueuedConnection);
    connect(&m_replayTimer, &QTimer::timeout, this, &ReplayProxy::replayStep);
    m_replayTimer.setInterval(1);
    m_replayTimer.setSingleShot(false);

    QTimer::singleShot(0, this, &ReplayProxy::readAll);
}

bool RecordProxy::recording() const {
    return m_logFile.isOpen() && m_logFile.isWritable() && m_logDataStream.status() == QDataStream::Ok;
}

void RecordProxy::onDataReceived(const QByteArray& data) {
    if (m_logFile.isOpen()) {
        QPair<QDateTime, QByteArray> newEvent {QDateTime::currentDateTime(), data};
        m_logDataStream << newEvent;
        m_logFile.flush();
        m_events.append(newEvent);
    }

    emit dataReceived(data);
}

RecordProxy::RecordProxy(QObject* parent)
    : BaseNetworkProxy(parent, Record)
    , m_creationTime(QDateTime::currentDateTime()) {
    QDir dir(getLogDirPath());
    if (!dir.exists()) {
        if (!dir.mkpath(QStringLiteral("."))) {
            qCritical() << QStringLiteral("Logger could not create folder %1 recording will be disabled.").arg(getLogDirPath());
            return;
        }
    }
    auto entries = dir.entryInfoList(QStringList {logFileNameTemplate.arg(QStringLiteral("*"))}, QDir::Filter::Files, QDir::SortFlag::Time);

    QString selectedFileName;
    // First check if all possible filenames are filled
    for (int i = 0; i < maxLogFiles; i++) {
        if (std::find_if(entries.begin(), entries.end(), [i](const auto& entry) {
                return entry.fileName() == BaseNetworkProxy::logFileNameTemplate.arg(i);
            }) == entries.end()) {
            slotSet(i);
            selectedFileName = QString {BaseNetworkProxy::logFileNameTemplate}.arg(i);
            break;
        }
    }

    // And if they are, just overwrite the oldest one
    if (selectedFileName.isEmpty()) {
        selectedFileName = entries.last().fileName();
        // Yes I realize this is an abomination and I've done it the third time now;
        auto justTheNumber = selectedFileName;
        justTheNumber.replace(QStringLiteral("LithReplay"), QStringLiteral());
        justTheNumber.replace(QStringLiteral(".dat"), QStringLiteral(""));
        slotSet(justTheNumber.toInt());
    }

    m_logFile.setFileName(dir.absoluteFilePath(selectedFileName));
    if (!m_logFile.open(QIODevice::WriteOnly)) {
        qCritical() << tr("Logger could not open %1 for writing, recording will be disabled.").arg(dir.absoluteFilePath(selectedFileName));
        return;
    }
    m_logDataStream.setDevice(&m_logFile);

    m_logDataStream << currentReplayVersion;
    m_logDataStream << m_creationTime;
}

QString ReplayRecordingInfo::erase() {
    QFile f(m_absolutePath);
    if (f.remove()) {
        deleteLater();
        return QStringLiteral("File \"%1\" was successfully erased.").arg(m_absolutePath);
    }
    return QStringLiteral("Could not erase file \"%1\": %2").arg(m_absolutePath).arg(f.errorString());
}

QString ReplayRecordingInfo::store() {
    QFile f(m_absolutePath);
    auto docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir docDir(docPath);
    auto newName =
        QStringLiteral("LithReplay%1_%2.dat").arg(m_number).arg(QDateTime::currentDateTime().toString(QStringLiteral("yyMMdd_hhmmss")));
    auto absoluteNewPath = docDir.absoluteFilePath(newName);
    if (f.copy(absoluteNewPath)) {
        return QStringLiteral("Replay %1 was copied to<br>\"<a href=\"%2\">%2</a>\".").arg(m_number).arg(absoluteNewPath);
    }
    return QStringLiteral("Could not save replay %1 to path \"%2\": %3").arg(m_number).arg(absoluteNewPath).arg(f.errorString());
}
