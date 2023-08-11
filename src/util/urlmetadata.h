#ifndef URLMETADATA_H
#define URLMETADATA_H

#include "common.h"

#include <QUrl>

class UrlMetaData : public QObject
{
    Q_OBJECT
    PROPERTY(QUrl, url)
    PROPERTY_READONLY_PRIVATESETTER(QString, mimeType)
    Q_PROPERTY(bool isImage READ isImage NOTIFY mimeTypeChanged)
    Q_PROPERTY(bool isVideo READ isVideo NOTIFY mimeTypeChanged)
    Q_PROPERTY(QUrl previewUrl READ previewUrl NOTIFY urlChanged)
public:
    explicit UrlMetaData(QObject *parent = nullptr);

    bool isImage() const;
    bool isVideo() const;

    QUrl previewUrl() const;

signals:

};

#endif // URLMETADATA_H
