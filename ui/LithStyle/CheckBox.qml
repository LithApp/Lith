import QtQuick 2.12
import QtQuick.Templates as T

T.CheckBox {
    id: control
    checked: true
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            Math.max(implicitContentWidth, indicator.width) + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    spacing: 0
    padding: 6

    background: Item {}

    indicator: Item {
        implicitWidth: 26 * Math.max(1.0, lith.settings.baseFontSize / 20)
        implicitHeight: implicitWidth
        anchors.verticalCenter: parent.verticalCenter
        Rectangle {
            radius: 3
            anchors.fill: parent
            color: palette.button
            Rectangle {
                id: shadow
                visible: control.enabled
                anchors.fill: parent
                radius: parent.radius
                opacity: control.pressed ? 0.12 : control.hovered ? 0.08 : 0.06
                property color startColor: control.pressed ? "black" : "white"
                Behavior on startColor { ColorAnimation { duration: 100 } }
                property color endColor: control.hovered && !control.pressed ? "white" : "transparent"
                Behavior on endColor { ColorAnimation { duration: 100 } }
                gradient: Gradient {
                    GradientStop { position: 0.0; color: shadow.startColor }
                    GradientStop { position: 0.6; color: shadow.endColor }
                }
            }
        }

        Item {
            anchors.centerIn: parent
            rotation: 45

            Rectangle {
                width: 18
                height: 4.5
                radius: 2
                anchors.centerIn: parent
                color: palette.windowText
                visible: control.checked
            }

            Rectangle {
                width: 4.5
                height: 18
                radius: 2
                anchors.centerIn: parent
                color: palette.windowText
                visible: control.checked
            }
        }
    }

    contentItem: Label {
        leftPadding: visible && control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: visible && control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0
        visible: text.length > 0
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: palette.windowText
        verticalAlignment: Text.AlignVCenter
    }
}
