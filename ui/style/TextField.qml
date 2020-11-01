import QtQuick 2.15
import QtQuick.Controls.Universal 2.15 as QQC2
import QtQuick.Templates 2.15 as T

QQC2.TextField {
    id: control
    implicitWidth: 200
    implicitHeight: 40

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 40
        color: control.enabled ? "transparent" : "#353637"
        border.color: control.enabled ? "#21be2b" : "transparent"
    }
}
