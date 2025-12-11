import QtQuick as QtQuick

QtQuick.Rectangle {
    id: root

    property bool checked: false
    property bool down: false
    property bool flat: false
    property bool hovered: false

    radius: 0

    color: root.flat ? "transparent" : root.palette.button
    state: ""

    border {
        color: Style.getOutlineColor(root.palette)
        width: root.flat ? 0 : 1;
    }

    states: [

        QtQuick.State {
            name: "checked"
            when: root.checked

            QtQuick.PropertyChanges {
                mask.opacity: 1
            }

        },

        QtQuick.State {
            name: "down"
            when: root.down

            QtQuick.PropertyChanges {
                mask.opacity: 0.5
            }

        },

        QtQuick.State {
            name: "hovered"
            when: root.hovered

            QtQuick.PropertyChanges {
                mask.opacity: 0.3
            }

        },

        QtQuick.State {
            name: ""

            QtQuick.PropertyChanges {
                mask.opacity: 0
            }

        }

    ]

    QtQuick.Rectangle {
        id: mask

        anchors.fill: parent
        antialiasing: true
        border.width: 0
        bottomLeftRadius: root.bottomLeftRadius
        bottomRightRadius: root.bottomRightRadius
        color: root.palette.highlight
        opacity: 0
        topLeftRadius: root.topLeftRadius
        topRightRadius: root.topRightRadius
    }

}
