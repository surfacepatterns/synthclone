import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.MenuItem {
    id: root

    anchors.left: parent.left
    anchors.right: parent.right

    bottomPadding: 5
    leftPadding: 3
    rightPadding: 3
    topPadding: 5

    spacing: 5

    implicitHeight:
        Math.max(
            Math.max(
                root.indicator.implicitHeight, root.contentItem.implicitHeight,
                root.arrow.implicitHeight) +
            root.topPadding + root.bottomPadding,
            root.implicitBackgroundHeight + root.topInset + root.bottomInset)
    implicitWidth:
        Math.max(
            root.indicator.implicitWidth + root.arrow.implicitWidth +
            root.contentItem.implicitWidth + root.leftPadding +
            root.rightPadding,
            root.implicitBackgroundWidth + root.leftInset + root.rightInset)

    icon.width: 16
    icon.height: 16

    property alias shortcuts: shortcut.sequences

    background: ButtonPanel {
        checked: root.checked
        down: root.down
        flat: true
        hovered: root.hovered
    }

    contentItem: QtQuick.Item {
        id: contentItem

        anchors.left:
            parent.mirrored ? parent.arrow.right : parent.indicator.right
        anchors.right:
            parent.mirrored ? parent.indicator.left : parent.arrow.left

        implicitHeight:
            Math.max(
                root.indicator.height, icon.height, text.height,
                shortcutLabel.height, root.arrow.height)
        implicitWidth:
            root.indicator.width + icon.width + text.width +
            shortcutLabel.width + root.arrow.width +
            (root.spacing * 4)

        // XXX: Mirroring isn't done on the label text.

        ThemeIcon {
            id: icon

            x:
                root.mirrored ?
                root.indicator.x - (icon.width + root.spacing) :
                (root.indicator.x + root.indicator.width) + root.spacing
            y: (root.availableHeight - icon.height) / 2

            cache: root.icon.cache
            enabled: root.enabled
            height: root.icon.height
            width: root.icon.width
            name: root.icon.name
        }

        Label {
            id: text

            x:
                root.mirrored ? icon.x - (text.width + root.spacing) :
                (icon.x + icon.width) + root.spacing
            y: (root.availableHeight - text.height) / 2

            verticalAlignment: QtQuick.Qt.AlignVCenter

            enabled: root.enabled
            font: root.font
            text: root.text
        }

        Label {
            id: shortcutLabel

            x:
                root.mirrored ?
                text.x - (shortcutLabel.width + root.spacing) :
                (text.x + text.width) + root.spacing
            y: (root.availableHeight - shortcutLabel.height) / 2

            anchors.right: root.mirrored ? text.left : parent.right

            horizontalAlignment: QtQuick.Qt.AlignRight
            verticalAlignment: QtQuick.Qt.AlignVCenter

            enabled: root.enabled
            font: root.font
            text: shortcut.nativeText
        }

    }

    QtQuick.Shortcut {
        id: shortcut
        enabled: root.enabled
        onActivated: root.triggered()
    }

}
