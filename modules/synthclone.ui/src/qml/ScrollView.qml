import QtQuick.Controls as Controls

Controls.ScrollView {
    id: root

    ScrollBar.horizontal: ScrollBar {
        id: horizontalBar

        //active: root.ScrollBar.vertical.active
        x: root.leftPadding
        y: root.height - horizontalBar.height
        width: root.availableWidth
    }

    ScrollBar.vertical: ScrollBar {
        id: verticalBar

        //active: root.ScrollBar.horizontal.active
        x: root.mirrored ? 0 : root.width - verticalBar.width
        y: root.topPadding
        height: root.availableHeight
    }

}
