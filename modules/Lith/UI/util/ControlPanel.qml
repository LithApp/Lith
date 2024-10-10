import QtQuick
import QtQuick.Shapes

import Lith.UI
import Lith.Style
import Lith.Core

Item {
    id: root
    clip: false

    property real topLeftRadius: radius
    property real topRightRadius: radius
    property real bottomLeftRadius: radius
    property real bottomRightRadius: radius

    property var relativePos: mapToItem(null, 0, 0, width, height)
    property real radius: 0
    property color color

    onXChanged: {
        root.relativePos = mapToItem(null, 0, 0, width, height)
    }
    onWidthChanged: {
        root.relativePos = mapToItem(null, 0, 0, width, height)
    }
    Component.onCompleted: {
        root.relativePos = mapToItem(null, 0, 0, width, height)
    }
    onVisibleChanged: {
        root.relativePos = mapToItem(null, 0, 0, width, height)
    }

    Shape {
        width: root.width
        height: root.height
        smooth: true
        antialiasing: true
        preferredRendererType: Shape.CurveRenderer
        ShapePath {
            strokeWidth: 0

            fillGradient: LinearGradient {
                id: lg
                x1: -root.relativePos.x
                x2: window.width + x1
                y1: -root.relativePos.y
                y2: window.height + y1

                GradientStop { position: 0.0; color: ColorUtils.mixColors(LithPalette.regular.alternateBase, LithPalette.regular.button, 0.1) }
                GradientStop { position: 0.5; color: ColorUtils.mixColors(LithPalette.regular.highlight, LithPalette.regular.window, 0.1) }
                GradientStop { position: 1.0; color: ColorUtils.mixColors(LithPalette.regular.alternateBase, LithPalette.regular.window, 0.1) }

            }
            startX: root.topLeftRadius; startY: 0
            PathLine { x: root.width - root.topRightRadius; y: 0}
            PathArc {
                x: root.width; y: root.topRightRadius
                radiusX: root.topRightRadius; radiusY: root.topRightRadius
            }
            PathLine { x: root.width; y: height - root.bottomRightRadius }
            PathArc {
                x: root.width - root.bottomRightRadius; y: root.height
                radiusX: root.bottomRightRadius; radiusY: root.bottomRightRadius
            }
            PathLine { x: root.bottomLeftRadius; y: root.height }
            PathArc {
                x: 0; y: root.height - root.bottomLeftRadius
                radiusX: root.bottomLeftRadius; radiusY: root.bottomLeftRadius
            }
            PathLine { x: 0; y: root.topLeftRadius }
            PathArc {
                x: root.topLeftRadius; y: 0
                radiusX: root.topLeftRadius; radiusY: root.topLeftRadius
            }
        }
        ShapePath {
            id: lightBorder
            strokeWidth: 0.5
            strokeColor: "#22cccccc"
            fillColor: "transparent"
            capStyle: ShapePath.FlatCap

            startX: strokeWidth / 2; startY: root.height - root.bottomLeftRadius
            PathLine { x: lightBorder.strokeWidth / 2; y: root.topLeftRadius }
            PathArc {
                x: root.topLeftRadius; y: lightBorder.strokeWidth / 2
                radiusX: root.topLeftRadius - lightBorder.strokeWidth / 2; radiusY: root.topLeftRadius - lightBorder.strokeWidth / 2
            }
            PathLine { x: root.width - root.topRightRadius; y: lightBorder.strokeWidth / 2}
        }

        ShapePath {
            id: midBorder
            strokeWidth: 0.5
            strokeColor: "#22888888"
            fillColor: "transparent"
            capStyle: ShapePath.FlatCap

            startX: root.width - root.topRightRadius; startY: midBorder.strokeWidth / 2
            PathArc {
                x: root.width - midBorder.strokeWidth / 2; y: root.topRightRadius
                radiusX: root.topRightRadius - midBorder.strokeWidth / 2; radiusY: root.topRightRadius - midBorder.strokeWidth / 2
            }
        }
        ShapePath {
            id: midBorder2
            strokeWidth: 0.5
            strokeColor: "#22888888"
            fillColor: "transparent"
            capStyle: ShapePath.FlatCap

            startX: root.bottomLeftRadius
            startY: root.height - midBorder2.strokeWidth / 2
            PathArc {
                x: midBorder2.strokeWidth / 2; y: root.height - root.bottomLeftRadius
                radiusX: root.bottomLeftRadius - midBorder2.strokeWidth / 2; radiusY: root.bottomLeftRadius - midBorder2.strokeWidth / 2
            }
        }

        ShapePath {
            id: darkBorder
            strokeWidth: 0.5
            strokeColor: "#221a1a1a"
            fillColor: "transparent"
            capStyle: ShapePath.FlatCap

            startX: root.width - darkBorder.strokeWidth / 2;
            startY: root.topRightRadius
            PathLine { x: root.width - darkBorder.strokeWidth / 2; y: root.height - root.bottomRightRadius}
            PathArc {
                x: root.width - root.bottomRightRadius; y: root.height - darkBorder.strokeWidth / 2
                radiusX: root.bottomRightRadius - darkBorder.strokeWidth / 2; radiusY: root.bottomRightRadius - darkBorder.strokeWidth / 2
            }
            PathLine { x: root.bottomLeftRadius; y: root.height - darkBorder.strokeWidth / 2}
        }
    }
}
