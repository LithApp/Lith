import QtQuick
import QtQuick.Layouts
import Lith.Style

Base {
    id: root
    property alias value: spinBox.value
    property alias from: spinBox.from
    property alias to: spinBox.to
    property alias stepSize: spinBox.stepSize
    property alias textFromValue: spinBox.textFromValue
    property alias valueFromText: spinBox.valueFromText

    rowComponent: SpinBox {
        id: spinBox
        Layout.fillWidth: true
        Layout.maximumWidth: root.halfWidth
        Layout.minimumWidth: spinBox.implicitWidth
    }
}
