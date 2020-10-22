#ifndef UPLOADER_H
#define UPLOADER_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>

class Uploader : public QObject
{
    Q_OBJECT
public:
    explicit Uploader(QObject *parent = nullptr);

public slots:
    void upload(const QString &path);
signals:
    void error(const QString &message);
    void success(const QString &url);

private slots:
    void onFinished(QNetworkReply *reply);
};

#endif // UPLOADER_H
