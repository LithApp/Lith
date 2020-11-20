#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>

class ScreenInfo : public QObject
{
    Q_OBJECT
public:
    explicit ScreenInfo(QObject *parent = nullptr);

    Q_INVOKABLE qreal getBottomSafeAreaSize();

signals:

};

#endif // SCREENINFO_H
