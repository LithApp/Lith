// Lith
// Copyright (C) 2020 Martin Bříza
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

#ifndef UPLOADER_H
#define UPLOADER_H

#include "common.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImage>

class Uploader : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    PROPERTY(bool, working)
public:
    explicit Uploader(QObject* parent = nullptr);
    static Uploader* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        return new Uploader(qmlEngine);
    }


public slots:
    void upload(const QString& path);
    void uploadBinary(QImage data);
signals:
    void error(const QString& message);
    void success(const QString& url);

private slots:
    void onFinished(QNetworkReply* reply);
};

#endif  // UPLOADER_H
