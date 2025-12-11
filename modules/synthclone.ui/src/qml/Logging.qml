pragma Singleton

import QtQuick as QtQuick

QtQuick.QtObject {

    readonly property QtQuick.LoggingCategory propertyCategory:
        QtQuick.LoggingCategory {
            name: "synthclone.ui.property"
            defaultLogLevel: QtQuick.LoggingCategory.Debug
        }

}
