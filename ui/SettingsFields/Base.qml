import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core

Rectangle {
    id: root

    property alias summary: summary.text
    property alias details: details.text
    property Item rowComponent
    property Item columnComponent
    readonly property Item labelComponent: summary
    readonly property real firstRowHeight: row.height
    readonly property real halfWidth: (root.width / 2) - horizontalPadding - (row.spacing / 2)

    readonly property real horizontalPadding: 9
    readonly property real verticalPadding: 3

    onRowComponentChanged: if (rowComponent) rowComponent.parent = row
    onColumnComponentChanged: if (columnComponent) columnComponent.parent = column

    Layout.fillWidth: true
    implicitWidth: column.implicitWidth + column.anchors.margins * 2
    implicitHeight: column.implicitHeight + root.verticalPadding * 2

    color: "transparent"
    border.width: 1
    border.color: palette.light

    ColumnLayout {
        id: column
        spacing: 0
        anchors {
            left: parent.left
            right: parent.right
            margins: root.horizontalPadding
        }
        y: root.verticalPadding
        height: parent.height - 2 * root.verticalPadding
        RowLayout {
            id: row
            Layout.minimumHeight: summary.font.pixelSize + 32
            Layout.fillWidth: true
            spacing: 3
            ColumnLayout {
                Layout.fillWidth: true
                Label {
                    id: summary
                    Layout.topMargin: details.visible ? 6 : 0
                    Layout.fillWidth: true
                    elide: Label.ElideRight

                    MouseArea {
                        id: baseMouse
                        anchors.fill: parent
                        hoverEnabled: true

                        ToolTip.text: summary.text + (details.text.length > 0 ? "\n" + details.text : "")
                        ToolTip.visible: window.platform.mobile ? baseMouse.containsPress : baseMouse.containsMouse
                        ToolTip.delay: window.platform.mobile ? 0 : 800
                    }
                }
                Label {
                    id: details
                    Layout.fillWidth: true
                    Layout.bottomMargin: visible ? 6 : 0
                    Layout.rightMargin: visible ? 6 : 0
                    elide: Label.ElideRight
                    font.pointSize: summary.font.pointSize * 0.7
                    opacity: 0.8
                    visible: text.length > 0
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    maximumLineCount: 4
                }
            }
        }
    }
}
