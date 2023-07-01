import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property alias summary: summary.text
    property alias details: details.text
    property Item rowComponent
    property Item columnComponent
    readonly property Item labelComponent: summary

    onRowComponentChanged: if (rowComponent) rowComponent.parent = row
    onColumnComponentChanged: if (columnComponent) columnComponent.parent = column

    Layout.fillWidth: true
    implicitWidth: column.implicitWidth
    implicitHeight: column.implicitHeight

    color: "transparent"
    border.width: 1
    border.color: palette.light

    ColumnLayout {
        id: column
        spacing: 0
        anchors {
            left: parent.left
            right: parent.right
            margins: 9
        }
        height: parent.height
        RowLayout {
            id: row
            Layout.minimumHeight: summary.font.pixelSize + 32
            Layout.fillWidth: true
            Layout.leftMargin: 6
            spacing: 3
            Label {
                id: summary
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                elide: Label.ElideRight
                Label {
                    id: details
                    width: parent.width
                    elide: Label.ElideRight
                    font.pointSize: summary.font.pointSize * 0.7
                    opacity: 0.8
                    visible: text.length > 0
                    anchors.top: summary.bottom
                    anchors.topMargin: -1
                }

                MouseArea {
                    id: baseMouse
                    anchors.fill: parent
                    hoverEnabled: true

                    ToolTip.text: summary.text + (details.text.length > 0 ? "\n" + details.text : "")
                    ToolTip.visible: window.platform.mobile ? baseMouse.containsPress : baseMouse.containsMouse
                    ToolTip.delay: window.platform.mobile ? 0 : 800
                }
            }
        }
    }
}
