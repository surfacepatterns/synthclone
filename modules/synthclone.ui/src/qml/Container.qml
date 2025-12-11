import QtQuick.Controls as Controls

Controls.Container {
    id: root

    implicitWidth:
        Math.max(
            root.implicitBackgroundWidth + root.leftInset + root.rightInset,
            root.implicitContentWidth + root.leftPadding + root.rightPadding)
    implicitHeight:
        Math.max(
            root.implicitBackgroundHeight + root.topInset + root.bottomInset,
            root.implicitContentHeight + root.topPadding + root.bottomPadding)

    // XXX: Setting these attributes appears to cause a binding loop.  I don't
    // understand enough about QML yet to understand why.
    contentHeight: root.contentItem.contentItem.childrenRect.height
    contentWidth: root.contentItem.contentItem.childrenRect.width
}
