import QtQuick as QtQuick

QtQuick.QtObject {
    id: root

    required property QtQuick.QtObject object
    required property string propertyName

    readonly property QtQuick.LoggingCategory _category:
        Logging.propertyCategory
    property var _value

    readonly property QtQuick.Binding _binding: QtQuick.Binding {
        target: root
        property: "_value"
        value: root.object[root.propertyName]
    }

    on_ValueChanged:
        console.log(
            root._category, `${object}.${root.propertyName} = ${root._value}`)

}
