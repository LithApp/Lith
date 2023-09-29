#include "liththeme.h"
#include <QtQuickTemplates2/private/qquicktheme_p.h>
#include <QFontInfo>

void LithTheme::initialize(QQuickTheme* theme) {
    qCritical() << "HYR" << theme << theme->font(QQuickTheme::Scope::System);
    QFont systemFont;
    QFont groupBoxTitleFont;
    QFont tabButtonFont;
    theme->setUsePlatformPalette(false);
    theme->setPalette(QQuickTheme::System, QPalette(Qt::red));

    const QFont font(QLatin1String("Segoe UI"));
    if (QFontInfo(font).family() == QLatin1String("Segoe UI")) {
        const QStringList families {font.family()};
        systemFont.setFamilies(families);
        groupBoxTitleFont.setFamilies(families);
        tabButtonFont.setFamilies(families);
    }

    systemFont.setPixelSize(15);
    theme->setFont(QQuickTheme::System, systemFont);

    groupBoxTitleFont.setPixelSize(15);
    groupBoxTitleFont.setWeight(QFont::DemiBold);
    theme->setFont(QQuickTheme::GroupBox, groupBoxTitleFont);

    tabButtonFont.setPixelSize(24);
    tabButtonFont.setWeight(QFont::Light);
    theme->setFont(QQuickTheme::TabBar, tabButtonFont);
    theme->setFont(QQuickTheme::Label, tabButtonFont);
}
