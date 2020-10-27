#include "uploader.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QHttpPart>
#include <QFile>
#include <QJsonDocument>
#include <QBuffer>
#include <QFileInfo>
#include <QImageReader>
#include <QApplication>
#include <QAbstractEventDispatcher>

Uploader::Uploader(QObject *parent) : QObject(parent)
{

}

void Uploader::upload(const QString &path) {
    QUrl url = QUrl("https://api.imgur.com/3/image");
    qApp->eventDispatcher()->processEvents(QEventLoop::AllEvents);

    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);

    QFile *file;
    if (path.startsWith("file://"))
        file = new QFile(QUrl(path).toLocalFile());
    else if (path.startsWith("file:assets-library")) {
        qCritical() << "1";
        QImageIOHandler::Transformations transformation;
        {
            // this needs to be in its own scope
            // because if it exists when I try to open the image, the whole thing freezes
            QImageReader imR(QUrl(path).toLocalFile());
            transformation = imR.transformation();
        }
        QImage im(QUrl(path).toLocalFile());
        if (transformation & QImageIOHandler::TransformationRotate90) {
            QMatrix matrix;
            matrix.translate(im.size().width() / 2, im.size().height() / 2);
            matrix.rotate(90);
            im = im.transformed(matrix);
        }
        uploadBinary(im);
        return;
    }
    else
        file = new QFile(path);
    if (!file->open(QIODevice::ReadOnly)) {
        emit error("Could not open file " + path);
        return;
    }

    workingSet(true);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", "Client-ID a416e89635996b4");

    qCritical() << "FILESIZE:" << file->size();
    auto reply = mgr->post(request, file->readAll());
    file->setParent(reply);

    connect(mgr, &QNetworkAccessManager::finished, this, &Uploader::onFinished);
}

void Uploader::uploadBinary(QImage &data) {
    QUrl url = QUrl("https://api.imgur.com/3/image");

    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", "Client-ID a416e89635996b4");

    workingSet(true);

    if (data.size().width() > 2000) {
        data = data.scaled(data.size() / 4);
    }
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    data.save(&buffer, "png");
    buffer.close();
    qCritical() << "BUFFER " << arr.size();
    qCritical() << "IMAGE " << data.size();
    auto reply = mgr->post(request, arr);

    connect(mgr, &QNetworkAccessManager::finished, this, &Uploader::onFinished);
}

void Uploader::onFinished(QNetworkReply *reply) {
    workingSet(false);
    if (reply->isReadable()) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        qCritical() << QString(doc.toJson());
        if (doc["success"].toBool()) {
            emit success(doc["data"]["link"].toString());
        }
        else {
            emit error(doc["data"]["error"].toString());
        }
    }
    else {
        emit error(reply->errorString());
    }
    reply->deleteLater();
}
