import QtQuick 2.11
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

import lith 1.0

Rectangle {
    width: ListView.view.width // + timeMetrics.width
    property var messageModel: null
    rotation: 180
    height: messageRootLayout.height

    color: messageModel.highlight ? "#22880000" : "transparent"

    SystemPalette {
        id: palette
    }

    MouseArea {
        anchors.fill: parent
        pressAndHoldInterval: 400 // does this do anything? or is it just for signals?
        hoverEnabled: false
        onPressAndHold: {
            // no rofl, tohle tady je, protoze "prefix" uz obsahuje barvicky
            // TODO: fakt nevim
            console.log(messageModel.colorlessNickname)

            channelMessageActionMenu.visible = true

            // TODO: how to do this better? neco s modelama? nevim jak to tam poslat, kdyz ten dialog mam jako jiny QML file :(
            channelMessageActionMenu.message = messageModel.colorlessText;

            channelMessageActionMenu.nickname = messageModel.colorlessNickname;
            channelMessageActionMenu.timestamp = messageModel.date.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
        }

    }
    RowLayout {
        id: messageRootLayout
        width: parent.width
        /*
        Text {
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: timeMetrics.width
            text: messageModel.date.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize
            color: palette.text
        }
        */
        Text {
            Layout.alignment: Qt.AlignTop
            font.bold: true
            text: messageModel.prefix
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize
            color: palette.text
        }

        Text {
            text: messageModel.message
            Layout.fillWidth: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: palette.text
            font.family: "Menlo"
            font.pointSize: settings.baseFontSize
            onLinkActivated: {
                linkHandler.show(link, this)
            }
            MouseArea {
                id: linkHoverMouse
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}
