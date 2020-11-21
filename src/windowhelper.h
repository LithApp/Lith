#ifndef WINDOWHELPER_H
#define WINDOWHELPER_H

#include "common.h"

class WindowHelper : public QObject
{
    Q_OBJECT
    PROPERTY_READONLY(bool, darkTheme, false)
    Q_PROPERTY(bool lightTheme READ lightThemeGet NOTIFY darkThemeChanged)
public:
    explicit WindowHelper(QObject *parent = nullptr);

    void init();

    bool lightThemeGet() { return !darkThemeGet(); }
    void resetColorScheme();

    qreal getBottomSafeAreaSize();

private:
    bool detectSystemDarkStyle();

signals:

};

#endif // WINDOWHELPER_H
