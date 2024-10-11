import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import Lith.Core
import Lith.Style

Rectangle {
    id: root

    property alias summary: summary.text
    property alias summaryComponent: summary
    property alias details: details.text
    property Item rowComponent
    property Item columnComponent
    readonly property Item labelComponent: summary
    readonly property real firstRowHeight: row.height
    readonly property real halfWidth: (root.width / 2) - (isNested ? 0 : horizontalPadding) - (row.spacing / 2)

    readonly property real horizontalPadding: 12
    readonly property real verticalPadding: 6

    onRowComponentChanged: if (rowComponent) rowComponent.parent = row
    onColumnComponentChanged: if (columnComponent) columnComponent.parent = column

    Layout.fillWidth: true
    implicitWidth: column.implicitWidth + column.anchors.margins * 2
    implicitHeight: column.implicitHeight + root.verticalPadding * 2

    color: ColorUtils.mixColors(LithPalette.regular.window, LithPalette.regular.base, 0.85)
    border.width: root.isNested ? 0 : 1
    border.color: LithPalette.regular.button
    readonly property int fieldIndex: parent.children.indexOf(this)
    readonly property Base previousSibling: parent.children[fieldIndex - 1] as Base
    readonly property Base nextSibling: parent.children[fieldIndex + 1] as Base
    property bool isNested: (parent as Base) || (parent?.parent as Base) || (parent?.parent?.parent as Base) || (parent?.parent?.parent?.parent as Base)

    topLeftRadius: previousSibling ? 0 : 4
    topRightRadius: previousSibling ? 0 : 4
    bottomLeftRadius: nextSibling ? 0 : 4
    bottomRightRadius: nextSibling ? 0 : 4

    ColumnLayout {
        id: column
        spacing: 0
        anchors {
            left: parent.left
            right: parent.right
            margins: root.isNested ? 0 : root.horizontalPadding
        }
        y: root.isNested ? 0 : root.verticalPadding
        height: parent.height - 2 * y
        RowLayout {
            id: row
            Layout.minimumHeight: summary.font.pixelSize + 24 + (root.isNested ? 0 : 12)
            Layout.fillWidth: true
            spacing: 3
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Label {
                    id: summary
                    Layout.topMargin: details.visible && !root.isNested ? 6 : 0
                    Layout.fillWidth: true
                    elide: Label.ElideRight
                    color: ColorUtils.mixColors(LithPalette.regular.windowText, LithPalette.regular.window, enabled ? 1.0 : 0.5)

                    MouseArea {
                        id: baseMouse
                        anchors.fill: parent
                        hoverEnabled: true

                        QQC2.ToolTip.text: summary.text + (details.text.length > 0 ? "\n" + details.text : "")
                        QQC2.ToolTip.visible: LithPlatform.mobile ? baseMouse.containsPress : baseMouse.containsMouse
                        QQC2.ToolTip.delay: LithPlatform.mobile ? 0 : 800
                    }
                }
                Label {
                    id: details
                    Layout.fillWidth: true
                    Layout.bottomMargin: visible && !root.isNested ? 6 : 0
                    Layout.rightMargin: visible ? 6 : 0
                    elide: Label.ElideRight
                    font.pixelSize: FontSizes.tiny
                    color: ColorUtils.mixColors(LithPalette.disabled.text, LithPalette.regular.window, enabled ? 1.0 : 0.5)
                    visible: text.length > 0
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    maximumLineCount: 6
                }
            }
        }
    }
}
