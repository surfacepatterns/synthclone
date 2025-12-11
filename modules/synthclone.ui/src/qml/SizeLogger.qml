import QtQuick as QtQuick

QtQuick.QtObject {
    id: root

    required property QtQuick.QtObject object

    readonly property list<PropertyLogger> _propertyLoggers: [

        // XXX: preferredHeight/preferredWidth from attached Layout?

        // Item, Popup, Window

        PropertyLogger {
            object: root.object
            propertyName: "height"
        },

        PropertyLogger {
            object: root.object
            propertyName: "width"
        },



        // Item, Popup

        PropertyLogger {
            object: root.object
            propertyName: "implicitHeight"
        },

        PropertyLogger {
            object: root.object
            propertyName: "implicitWidth"
        },



        // Control, Popup

        PropertyLogger {
            object: root.object
            propertyName: "availableHeight"
        },

        PropertyLogger {
            object: root.object
            propertyName: "availableWidth"
        },

        PropertyLogger {
            object: root.object
            propertyName: "implicitContentHeight"
        },

        PropertyLogger {
            object: root.object
            propertyName: "implicitContentWidth"
        },



        // Control, Popup, Label

        PropertyLogger {
            object: root.object
            propertyName: "implicitBackgroundHeight"
        },

        PropertyLogger {
            object: root.object
            propertyName: "implicitBackgroundWidth"
        },



        // Container, Pane, Popup, Text

        PropertyLogger {
            object: root.object
            propertyName: "contentHeight"
        },

        PropertyLogger {
            object: root.object
            propertyName: "contentWidth"
        },



        // Window

        PropertyLogger {
            object: root.object
            propertyName: "maximumHeight"
        },

        PropertyLogger {
            object: root.object
            propertyName: "maximumWidth"
        },

        PropertyLogger {
            object: root.object
            propertyName: "minimumHeight"
        },

        PropertyLogger {
            object: root.object
            propertyName: "minimumWidth"
        }

    ]

}
