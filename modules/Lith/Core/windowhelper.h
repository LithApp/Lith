#ifndef WINDOWHELPER_H
#define WINDOWHELPER_H

#include "common.h"
#include "lithcore_export.h"

#include <QQuickWindow>

class LITHCORE_EXPORT WindowHelper : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    PROPERTY_READONLY(bool, darkTheme, false)
    PROPERTY_READONLY(bool, useBlack, false)
    Q_PROPERTY(bool lightTheme READ lightThemeGet NOTIFY darkThemeChanged)
    Q_PROPERTY(ColorTheme currentTheme READ currentTheme NOTIFY darkThemeChanged)
public:
    explicit WindowHelper(QObject* parent = nullptr);

    void init();

    bool lightThemeGet() const {
        return !darkThemeGet();
    }
    const ColorTheme& currentTheme() const;
    const ColorTheme& inverseTheme() const;

    Q_INVOKABLE static QVariantMap getSafeAreaMargins(QQuickWindow* window);

private:
    static bool detectSystemDarkStyle();

signals:
    void themeChanged();
};

#endif  // WINDOWHELPER_H
