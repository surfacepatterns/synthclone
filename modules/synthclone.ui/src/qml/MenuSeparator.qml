import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.MenuSeparator {
    id: root
    bottomPadding: 7
    leftPadding: 10
    topPadding: 7
    rightPadding: 10

    contentItem: QtQuick.Rectangle {
        color: Style.getOutlineColor(root.palette)
        height: 0.5
        implicitHeight: 0.5
    }

}
