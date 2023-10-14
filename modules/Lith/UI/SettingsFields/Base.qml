import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Lith.Core
import Lith.Style

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
    border.color: LithPalette.regular.light

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
            Layout.minimumHeight: summary.font.pixelSize + 36
            Layout.fillWidth: true
            spacing: 3
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Label {
                    id: summary
                    Layout.topMargin: details.visible ? 6 : 0
                    Layout.fillWidth: true
                    elide: Label.ElideRight
                    color: ColorUtils.mixColors(LithPalette.regular.windowText, LithPalette.regular.window, enabled ? 1.0 : 0.5)

                    MouseArea {
                        id: baseMouse
                        anchors.fill: parent
                        hoverEnabled: true

                        ToolTip.text: summary.text + (details.text.length > 0 ? "\n" + details.text : "")
                        ToolTip.visible: LithPlatform.mobile ? baseMouse.containsPress : baseMouse.containsMouse
                        ToolTip.delay: LithPlatform.mobile ? 0 : 800
                    }
                }
                Label {
                    id: details
                    Layout.fillWidth: true
                    Layout.bottomMargin: visible ? 6 : 0
                    Layout.rightMargin: visible ? 6 : 0
                    elide: Label.ElideRight
                    font.pixelSize: summary.font.pixelSize * 0.7
                    color: ColorUtils.mixColors(LithPalette.disabled.text, LithPalette.regular.window, enabled ? 1.0 : 0.5)
                    visible: text.length > 0
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    maximumLineCount: 4
                }
            }
        }
    }
}
