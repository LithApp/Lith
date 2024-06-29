// Lith
// Copyright (C) 2020 Martin Bříza
// Copyright (C) 2020 Václav Kubernát
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

#include "uploader.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QFile>
#include <QJsonDocument>
#include <QBuffer>
#include <QFileInfo>
#include <QImageReader>
#include <QApplication>
#include <QAbstractEventDispatcher>

#include "lith.h"

Uploader::Uploader(QObject* parent)
    : QObject(parent) {
}

void Uploader::upload(const QString& path) {
    auto url = QUrl(QStringLiteral("https://api.imgur.com/3/image"));
    qApp->eventDispatcher()->processEvents(QEventLoop::AllEvents);

    auto* mgr = new QNetworkAccessManager(this);

    QFile* file = nullptr;
    if (path.startsWith(QStringLiteral("file://"))) {
        file = new QFile(QUrl(path).toLocalFile());
    } else if (path.startsWith(QStringLiteral("file:assets-library"))) {
        QImageIOHandler::Transformations transformation;
        {
            // this needs to be in its own scope
            // because if it exists when I try to open the image, the whole thing freezes
            QImageReader imR(QUrl(path).toLocalFile());
            transformation = imR.transformation();
        }
        QImage im(QUrl(path).toLocalFile());
        if (transformation == QImageIOHandler::TransformationRotate90) {
            QTransform transform;
            transform.translate(im.size().width() / 2.0, im.size().height() / 2.0);
            transform.rotate(90);
            im = im.transformed(transform);
        } else if (transformation == QImageIOHandler::TransformationRotate180) {
            QTransform transform;
            transform.translate(im.size().width() / 2.0, im.size().height() / 2.0);
            transform.rotate(180);
            im = im.transformed(transform);
        } else if (transformation == QImageIOHandler::TransformationRotate270) {
            QTransform transform;
            transform.translate(im.size().width() / 2.0, im.size().height() / 2.0);
            transform.rotate(270);
            im = im.transformed(transform);
        }
        uploadBinary(im);
        return;
    } else {
        file = new QFile(path);
    }
    if (!file->open(QIODevice::ReadOnly)) {
        emit error(QStringLiteral("Could not open file %1").arg(path));
        return;
    }

    workingSet(true);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/application/x-www-form-urlencoded"));
    request.setRawHeader("Authorization", "Client-ID " + Lith::settingsGet()->imgurApiKeyGet().toUtf8());

    auto* reply = mgr->post(request, file->readAll());
    file->setParent(reply);

    connect(mgr, &QNetworkAccessManager::finished, this, &Uploader::onFinished);
}

void Uploader::uploadBinary(QImage data) {
    auto url = QUrl(QStringLiteral("https://api.imgur.com/3/image"));

    auto* mgr = new QNetworkAccessManager(this);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/application/x-www-form-urlencoded"));
    request.setRawHeader(
        QByteArrayLiteral("Authorization"), QByteArrayLiteral("Client-ID ") + Lith::settingsGet()->imgurApiKeyGet().toUtf8()
    );

    workingSet(true);

    if (data.size().width() > 2500) {
        data = data.scaled(data.size() / 2.0);
    }
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    data.save(&buffer, "PNG");
    buffer.close();
    auto* reply = mgr->post(request, arr);

    connect(mgr, &QNetworkAccessManager::finished, this, &Uploader::onFinished);
}

void Uploader::onFinished(QNetworkReply* reply) {
    workingSet(false);
    if (reply->isReadable()) {
        auto doc = QJsonDocument::fromJson(reply->readAll());
        if (doc[QStringLiteral("success")].toBool()) {
            emit success(doc[QStringLiteral("data")][QStringLiteral("link")].toString());
        } else {
            emit error(doc[QStringLiteral("data")][QStringLiteral("error")].toString());
        }
    } else {
        emit error(reply->errorString());
    }
    reply->deleteLater();
}
