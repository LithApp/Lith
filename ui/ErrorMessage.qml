import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Rectangle {
    height: errorText.text.length > 0 ? 64 : 0
    width: parent.width
    visible: height != 0
    gradient: Gradient {
        GradientStop { position: 0.0; color: "dark red" }
        GradientStop { position: 0.22; color: "red" }
    }
    Behavior on height {
        NumberAnimation {
            duration: 120
            easing.type: Easing.InOutQuart
        }
    }
    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 18
        Text {
            Layout.fillHeight: true
            Layout.fillWidth: true
            verticalAlignment: Text.AlignVCenter
            id: errorText
            text: lith.errorString
        }
        Button {
            text: "x"
            Layout.preferredWidth: 28
            Layout.preferredHeight: 36
            onClicked: lith.errorString = ""
        }
    }
}
