#include "lithstyle.h"
#include "liththeme.h"

#include <QtQuickControls2/private/qquickstyleplugin_p.h>
#include <QtQuickTemplates2/private/qquicktheme_p.h>

extern void qml_register_types_Lith_Style();
Q_GHS_KEEP_REFERENCE(qml_register_types_Lith_Style);

class Lith_StylePlugin : public QQuickStylePlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    Lith_StylePlugin(QObject* parent = nullptr);

    QString name() const override;
    void initializeTheme(QQuickTheme* theme) override;

    LithTheme theme;
};

Lith_StylePlugin::Lith_StylePlugin(QObject* parent)
    : QQuickStylePlugin(parent) {
    volatile auto registration = &qml_register_types_Lith_Style;
    Q_UNUSED(registration);
}

QString Lith_StylePlugin::name() const {
    return QStringLiteral("Lith.Style");
}

void Lith_StylePlugin::initializeTheme(QQuickTheme* theme) {
    LithStyle::initialize();
    this->theme.initialize(theme);
}

#include "lith_styleplugin.moc"
