#include "urlmetadata.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMimeType>

UrlMetaData::UrlMetaData(QObject *parent)
    : QObject{parent}
{
    connect(this, &UrlMetaData::urlChanged, this, [this]() {
        qCritical() << "URL" << urlGet().host();
        QNetworkRequest request(urlGet());
        auto reply = (new QNetworkAccessManager(this))->head(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            qCritical() << reply->header(QNetworkRequest::ContentTypeHeader);
            mimeTypeSet(reply->header(QNetworkRequest::ContentTypeHeader).toString());
            reply->deleteLater();
        });
    });
}

bool UrlMetaData::isImage() const {
    return mimeTypeGet().startsWith("image");
}

bool UrlMetaData::isVideo() const {
    return mimeTypeGet().startsWith("video");
}

QUrl UrlMetaData::previewUrl() const {
    auto host = urlGet().host();
    if (host.endsWith("imgur.com")) {
        QUrl copy(urlGet());
        auto filename = urlGet().fileName();
        auto dotIndex = filename.indexOf('.');
        if (dotIndex >= 0) {
            filename = filename.left(filename.indexOf('.'));
        }
        filename.append("l.png");
        qCritical() << "pre-copy:" << copy;
        copy.setPath("/"+filename);
        qCritical() << "FILENAME:" << filename << "copy:" << copy;

        return copy;
    }
    return urlGet();
}
