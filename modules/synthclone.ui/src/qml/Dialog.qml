import QtQuick as QtQuick
import QtQuick.Controls as Controls

Controls.Dialog {
    id: root

    property alias buttons: buttonBox.contentChildren

    closePolicy: Popup.CloseOnEscape
    modal: true
    padding: 0
    popupType: Controls.Popup.Window
    visible: false

    footer: DialogButtonBox {
        id: buttonBox
        visible: buttonBox.contentChildren.length > 0
    }

}
