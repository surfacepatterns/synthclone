import QtQuick as QtQuick

QtQuick.Rectangle {
    id: root

    color: "transparent"
    height: 31
    implicitHeight: 31

    QtQuick.Rectangle {
        anchors.centerIn: root
        color: Style.getOutlineColor(root.palette)
        height: 1
        implicitHeight: 1
        width: root.width
    }

}
