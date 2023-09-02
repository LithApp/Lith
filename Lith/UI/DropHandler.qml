import QtQuick
import QtQuick.Controls

DropArea {
    id: dropArea
    anchors.fill: parent
    onEntered: {
        dropAreaLabel.text = ""
        dropAreaPreview.source = ""
        if (drag.hasUrls) {
            dropAreaLabel.text = "<b>Pasting this file:</b><br>"
            dropAreaLabel.text += drag.urls[0].replace(/^.*[\\\/]/, '')
            dropAreaPreview.source = drag.urls[0]
        }
        else if (drag.hasText) {
            dropAreaLabel.text = "<b>Pasting this text:</b><br>"
            dropAreaLabel.text += drag.text
        }
        else {
            dropAreaLabel.text = "Unhandled drag and drop format, sorry"
        }
    }
    onDropped: {
        if (drop.hasUrls) {
            dropArea.urlEntered(drop.urls[0])
        }
        else if (drop.hasText) {
            dropArea.textEntered(drop.text)
        }
        else {
            dropAreaLabel.text = "Unhandled drag and drop format, sorry"
        }
    }

    signal textEntered(string t)
    signal urlEntered(url u)


    Rectangle {
        anchors.fill: parent
        anchors.margins: 6
        radius: 3
        opacity: dropArea.containsDrag ? 0.9 : 0.0
        Behavior on opacity { NumberAnimation { duration: 60 } }
        color: palette.alternateBase
        Column {
            spacing: 9
            anchors.centerIn: parent
            width: parent.width
            Label {
                id: dropAreaLabel
                textFormat: Text.RichText
                width: parent.width
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Qt.AlignHCenter
            }
            Image {
                id: dropAreaPreview
                width: parent.width * 0.7
                fillMode: Image.PreserveAspectFit
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
