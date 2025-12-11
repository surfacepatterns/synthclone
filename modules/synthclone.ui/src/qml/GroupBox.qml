import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.GroupBox {
    id: root

    height: root.implicitHeight + root.implicitLabelHeight
    bottomPadding: 0
    leftPadding: 15
    rightPadding: 15
    topPadding: root.implicitLabelHeight + 15

    background: QtQuick.Rectangle {
        color: "transparent"
    }

    label: Label {
        text: root.title
        font.weight: QtQuick.Font.DemiBold
    }

}
