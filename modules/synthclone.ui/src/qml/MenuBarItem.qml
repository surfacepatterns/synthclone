import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.MenuBarItem {
    id: root

    property alias shortcuts: shortcut.sequences

    background: ButtonPanel {
        checked: root.checked
        down: root.down
        flat: true
        hovered: root.hovered || root.highlighted
    }

    contentItem: Label {
        color: root.checked ? root.palette.highlightedText : root.palette.text
        font: root.font
        text: root.text
    }

    QtQuick.Shortcut {
        id: shortcut
        enabled: root.enabled
        onActivated: root.triggered()
    }

}
