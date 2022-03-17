#ifndef WINDOWHELPER_H
#define WINDOWHELPER_H

#include "common.h"

class WindowHelper : public QObject
{
    Q_OBJECT
    PROPERTY_READONLY(bool, darkTheme, false)
    PROPERTY_READONLY(bool, useBlack, false)
    Q_PROPERTY(bool lightTheme READ lightThemeGet NOTIFY darkThemeChanged)
    Q_PROPERTY(ColorTheme currentTheme READ currentTheme NOTIFY darkThemeChanged)
public:
    explicit WindowHelper(QObject *parent = nullptr);

    void init();

    bool lightThemeGet() { return !darkThemeGet(); }
    const ColorTheme &currentTheme();
    const ColorTheme &inverseTheme();

    Q_INVOKABLE qreal getBottomSafeAreaSize();

private:
    bool detectSystemDarkStyle();

signals:
    void themeChanged();

};

#endif // WINDOWHELPER_H
