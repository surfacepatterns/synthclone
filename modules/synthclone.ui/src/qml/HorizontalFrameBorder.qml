import QtQuick as QtQuick

QtQuick.Rectangle {
    id: root

    antialiasing: true
    color: Style.getOutlineColor(root.palette)
    height: 1
    implicitHeight: 1
}
