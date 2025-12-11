import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.Button {
    id: root

    font.weight: QtQuick.Font.Medium
    hoverEnabled: root.enabled
    icon.color: "transparent"
    icon.height: 24
    icon.width: 24
    state: ""

    bottomPadding: 5
    leftPadding: 10
    rightPadding: 10
    topPadding: 5

    spacing: 5

    background: ButtonPanel {
        id: panel

        checked: root.checked
        down: root.down
        flat: root.flat
        hovered: root.hovered

        radius: 3
    }

    contentItem: QtQuick.Item {
        id: contentItem

        anchors.fill: parent

        implicitHeight: Math.max(icon.height, text.height)
        implicitWidth: icon.width + text.width + root.spacing

        ThemeIcon {
            id: icon

            x:
                root.mirrored ? text.width + root.spacing + root.leftPadding :
                root.leftPadding
            y:
                root.topPadding +
                ((root.availableHeight - icon.height) / 2)

            cache: root.icon.cache
            enabled: root.enabled
            name: root.icon.name
            height: root.icon.height
            width: root.icon.width
        }

        Label {
            id: text

            x:
                root.mirrored ? root.leftPadding :
                icon.width + root.spacing + root.leftPadding
            y:
                root.topPadding +
                ((root.availableHeight - text.height) / 2)

            text: root.text
            font: root.font
            color: root.palette.buttonText

            horizontalAlignment: QtQuick.Text.AlignLeft
            verticalAlignment: QtQuick.Text.AlignVCenter

        }

    }

}
