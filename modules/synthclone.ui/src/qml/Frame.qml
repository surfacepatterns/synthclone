import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.Frame {
    id: root

    background: QtQuick.Rectangle {
        border.color: Style.getOutlineColor(root.palette)
        color: "transparent"
        radius: 3
    }

}
