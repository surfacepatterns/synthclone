import QtQuick as QtQuick

Pane {
    id: root

    padding: 0

    background: QtQuick.Rectangle {
        id: backgroundRect

        color: Style.getContentColor(root.palette)
    }

}
