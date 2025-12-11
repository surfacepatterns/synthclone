import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.Menu {
    id: root

    bottomPadding: backgroundRect.radius
    leftPadding: backgroundRect.border.width
    rightPadding: backgroundRect.border.width
    topPadding: backgroundRect.radius

    background: QtQuick.Rectangle {
        id: backgroundRect

        border.color: Style.getOutlineColor(root.palette)
        color: Style.getContentColor(root.palette)
        radius: 3
        implicitWidth: 200
        implicitHeight: 20
    }

    contentItem: QtQuick.ListView {
        id: listView

        model: root.contentModel
        currentIndex: root.currentIndex
        spacing: root.spacing
        width: root.width
        orientation: QtQuick.ListView.Vertical
        boundsBehavior: QtQuick.Flickable.StopAtBounds
        flickableDirection: QtQuick.Flickable.AutoFlickIfNeeded
        snapMode: QtQuick.ListView.SnapToItem
        highlightMoveDuration: 0
        highlightRangeMode: QtQuick.ListView.ApplyRange
        interactive: false

        implicitHeight: listView.contentItem.childrenRect.height
        implicitWidth:
            listView.contentItem.children.reduce(
                (width, item) => Math.max(width, item.implicitWidth), 0)

    }

}
