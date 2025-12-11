import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.TabButton {
    id: root

    property int cornerRadius: 0

    font.weight: QtQuick.Font.Medium
    hoverEnabled: root.enabled
    state: ""
    z: 0

    bottomPadding: 10
    leftPadding: 10
    rightPadding: 10
    topPadding: 10

    background: ButtonPanel {
        antialiasing: true
        bottomLeftRadius: 0
        bottomRightRadius: 0
        topLeftRadius: root.cornerRadius
        topRightRadius: root.cornerRadius

        checked: root.checked
        down: root.down
        flat: true
        hovered: root.hovered
    }

    contentItem: Label {
        id: label

        text: root.text
        font: root.font
        color:
            root.checked ? root.palette.highlightedText :
            root.palette.buttonText

        horizontalAlignment: QtQuick.Text.AlignHCenter
        verticalAlignment: QtQuick.Text.AlignVCenter
    }

}
