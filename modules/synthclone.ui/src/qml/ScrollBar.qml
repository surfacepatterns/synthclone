import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.ScrollBar {
    id: root

    interactive: true
    snapMode: Controls.ScrollBar.NoSnap

    contentItem: QtQuick.Rectangle {
        id: handle

        border.color: Style.getOutlineColor(root.palette)
        border.width: 1.5
        color: root.pressed ? root.palette.highlight : root.palette.dark
        implicitHeight: 10
        implicitWidth: 10
        radius: handle.width / 2

        visible:
            (root.policy == ScrollBar.AlwaysOn) ||
            ((root.policy == ScrollBar.AsNeeded) && (root.size < 1.0))

    }

}
