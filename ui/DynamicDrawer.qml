import QtQuick

Rectangle {
    id: drawer
    property bool isClosed: position === 0.0
    readonly property bool closeCompletely: false
    readonly property real closedLimit: closeCompletely ? 0 : 36
    width: visible ? landscapeMode ? dragHandle.x + (dragHandle.width / 2) : 0.66 * mainView.width + mainView.leftMargin : closedLimit
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
        value: -drawer.width + closedLimit
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
            drawer.x = -drawer.width + drawer.closedLimit
        dragOutHandle.visible = false
        openAnimation.start()
    }
    function hide() {
        //x = -width
        if (lastState != DynamicDrawer.State.Closed) {
            lastState = DynamicDrawer.State.Closed
            if (!dragOutHandleMouse.drag.active)
                drawer.x = 0
            closeAnimation.start()
            dragOutHandle.visible = true
        }
    }

    readonly property real position: 1 - Math.abs((x) / (width - closedLimit))
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
        to: -drawer.width + drawer.closedLimit
        duration: 120
        onFinished: lastState = DynamicDrawer.State.Closed
        running: false
    }

    Rectangle {
        //visible: !drawer.isClosed
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            topMargin: drawer.dragOutHandleTopMargin
            bottomMargin: drawer.dragOutHandleBottomMargin
            rightMargin: -1
        }
        width: 1
        color: palette.text
        opacity: 0.5
    }

    Rectangle {
        id: backdrop
        visible: !landscapeMode && opacity > 0.0
        opacity: drawer.position
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
            onClicked: {
                console.warn("backdrop")
                drawer.hide()
            }
        }
    }

    Rectangle {
        id: dragHandle
        visible: landscapeMode && !drawer.isClosed
        z: 1
        x: 320
        width: 11
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        color: palette.text
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
            onClicked: console.warn("dragHandleMouse")
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
        z: 1000

        MouseArea {
            id: dragOutHandleMouse
            anchors.fill: parent
            anchors.leftMargin: -drawer.closedLimit
            propagateComposedEvents: true
            drag.axis: Drag.XAxis
            drag.target: drawer
            drag.minimumX: -drawer.width + drawer.closedLimit
            drag.maximumX: 0
            drag.onActiveChanged: {
                if (drawer.x < -drawer.width + dragOutHandle.threshold) {
                    drawer.hide()
                }
                else {
                    drawer.open()
                }
            }

            /*
            onPressed: (mouse) => {
                           mouse.accepted = false
                       }

            onReleased: function(mouse) {
                mouse.accepted = false
                if (drawer.x < -drawer.width + dragOutHandle.threshold) {
                    drawer.hide()
                }
                else {
                    drawer.open()
                }
            }
            */
            onClicked: (mouse) => {
                console.warn("dragOutHandleMouse")
                mouse.accepted = false
            }
        }
    }
}
