import QtQuick as QtQuick

Container {
    id: root

    required property int orientation

    currentIndex: listView.currentIndex
    spacing: 0

    background: QtQuick.Rectangle {
        color: "transparent"
    }

    contentItem: QtQuick.ListView {
        id: listView

        model: root.contentModel
        currentIndex:
            buttonGroup.checkedButton != null ?
            buttonGroup.buttons.indexOf(buttonGroup.checkedButton) : -1
        spacing: root.spacing
        width: root.width
        orientation: root.orientation
        boundsBehavior: QtQuick.Flickable.StopAtBounds
        flickableDirection: QtQuick.Flickable.AutoFlickIfNeeded
        snapMode: QtQuick.ListView.SnapToItem
        highlightMoveDuration: 0
        highlightRangeMode: QtQuick.ListView.ApplyRange
        interactive: false
    }

    QtQuick.Component.onCompleted: {
        if (
            (buttonGroup.checkedButton === null) &&
            buttonGroup.buttons.length
        ) {
            buttonGroup.buttons[0].checked = true;
        }
    }

    ButtonGroup {
        id: buttonGroup

        buttons: root.contentChildren
    }

}
