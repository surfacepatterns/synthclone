import QtQuick as QtQuick

QtQuick.Rectangle {
    id: root

    antialiasing: true
    color: Style.getOutlineColor(root.palette)
    implicitWidth: 1
    width: 1
}
