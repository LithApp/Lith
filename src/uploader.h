#ifndef UPLOADER_H
#define UPLOADER_H

#include "common.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImage>

class Uploader : public QObject
{
    Q_OBJECT
    PROPERTY(bool, working)
public:
    explicit Uploader(QObject *parent = nullptr);

public slots:
    void upload(const QString &path);
    void uploadBinary(QImage &data);
signals:
    void error(const QString &message);
    void success(const QString &url);

private slots:
    void onFinished(QNetworkReply *reply);
};

#endif // UPLOADER_H
