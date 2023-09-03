import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts

Base {
    id: root
    property alias text: button.text
    signal clicked

    rowComponent: QQC.Button {
        id: button
        onClicked: root.clicked()
    }
}
