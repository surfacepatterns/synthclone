import QtQuick as QtQuick

Label {
    id: root

    required property double fontScale
    required property double marginBlockScale

    readonly property double _fontSize:
        QtQuick.Application.font.pointSize * root.fontScale
    readonly property double _marginBlockSize:
        Math.round(
            (QtQuick.Application.font.pixelSize * root.fontScale) *
            root.marginBlockScale)

    font.weight: QtQuick.Font.DemiBold
    font.pointSize: root._fontSize

    bottomInset: root._marginBlockSize
    bottomPadding: root._marginBlockSize
    topInset: root._marginBlockSize
    topPadding: root._marginBlockSize
}
