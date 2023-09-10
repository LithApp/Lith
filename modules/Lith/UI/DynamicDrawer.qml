import QtQuick

import Lith.Core
import Lith.Style

Rectangle {
    id: drawer
    property bool isClosed: position === 0.0
    width: visible ? WindowHelper.landscapeMode ? dragHandle.x + (dragHandle.width / 2) : 0.66 * mainView.width + mainView.leftMargin : 0
    property real dragOutHandleTopMargin: 0
    property real dragOutHandleBottomMargin: 0

    enum State {
        None,
        Open,
        Closed
    }

    property int lastState: DynamicDrawer.State.Closed

    Binding {
        when: !dragOutHandleMouse.drag.active &&
              drawer.lastState === DynamicDrawer.State.Closed
        target: drawer
        property: "x"
        value: -drawer.width
    }
    Binding {
        when: !dragOutHandleMouse.drag.active &&
              drawer.lastState === DynamicDrawer.State.Open
        target: drawer
        property: "x"
        value: 0
    }

    function toggle() {
        if (isClosed)
            open()
        else
            hide()
    }
    function open() {
        //x = 0
        if (position > 0.999) {
            lastState = DynamicDrawer.State.Open
            return
        }
        lastState = DynamicDrawer.State.None
        if (!dragOutHandleMouse.drag.active)
            drawer.x = -drawer.width
        dragOutHandle.visible = false
        openAnimation.start()
    }
    function hide() {
        //x = -width
        lastState = DynamicDrawer.State.None
        if (!dragOutHandleMouse.drag.active)
            drawer.x = 0
        closeAnimation.start()
        dragOutHandle.visible = true
    }

    readonly property real position: 1 - Math.abs(x / width)
    onPositionChanged: {
        if (position === 0.0)
            dragOutHandle.visible = true
    }
    NumberAnimation on x {
        id: openAnimation
        to: 0
        duration: 120
        onFinished: lastState = DynamicDrawer.State.Open
        running: false
    }
    NumberAnimation on x {
        id: closeAnimation
        to: -drawer.width
        duration: 120
        onFinished: lastState = DynamicDrawer.State.Closed
        running: false
    }

    Rectangle {
        visible: !drawer.isClosed
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            rightMargin: -1
        }
        width: 1
        color: ColorUtils.mixColors(LithPalette.regular.text, LithPalette.regular.window, 0.5)
    }

    Rectangle {
        id: backdrop
        visible: !WindowHelper.landscapeMode && opacity > 0.0
        opacity: bufferDrawer.position
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.right
        }
        width: 99999
        color: "#88000000"
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.ArrowCursor
            acceptedButtons: Qt.AllButtons
            propagateComposedEvents: false
            onClicked: bufferDrawer.hide()
        }
    }

    Rectangle {
        id: dragHandle
        visible: WindowHelper.landscapeMode && !drawer.isClosed
        z: 1
        x: 320
        width: 11
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        color: LithPalette.regular.text
        opacity: dragHandleMouse.drag.active ? 0.35 : dragHandleMouse.containsMouse ? 0.25 : 0.0
        Behavior on opacity {
            NumberAnimation {
                duration: 120
            }
        }

        MouseArea {
            id: dragHandleMouse
            anchors.fill: parent
            cursorShape: Qt.SizeHorCursor
            hoverEnabled: true

            drag.target: dragHandle
            drag.axis: Drag.XAxis
            drag.minimumX: 200
            drag.maximumX: window.width - 200
        }
    }

    Item {
        id: dragOutHandle
        anchors{
            top: parent.top
            bottom: parent.bottom
            topMargin: drawer.dragOutHandleTopMargin
            bottomMargin: drawer.dragOutHandleBottomMargin
        }
        x: parent.width - 32
        property real threshold: 100
        width: 64

        MouseArea {
            id: dragOutHandleMouse
            anchors.fill: parent
            propagateComposedEvents: true
            drag.axis: Drag.XAxis
            drag.target: bufferDrawer
            drag.minimumX: -bufferDrawer.width
            drag.maximumX: 0
            onReleased: function(mouse) {
                if (bufferDrawer.x < -bufferDrawer.width + dragOutHandle.threshold) {
                    drawer.hide()
                }
                else {
                    drawer.open()
                }
            }
        }
    }
}
