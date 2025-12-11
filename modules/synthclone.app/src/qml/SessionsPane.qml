import QtQuick as QtQuick
import QtQuick.Layouts as Layouts

import Synthclone.Ui as Ui

Layouts.ColumnLayout {

    Ui.H1 {
        text: "Sessions"
    }

    Layouts.ColumnLayout {
        Layouts.Layout.alignment: QtQuick.Qt.AlignLeft
        Layouts.Layout.fillWidth: false

        Ui.Button {
            Layouts.Layout.alignment: QtQuick.Qt.AlignLeft
            Layouts.Layout.fillWidth: true

            flat: true
            font.weight: QtQuick.Font.Medium;
            icon.name: "document-new"
            icon.width: 32
            icon.height: 32
            text: "Create new session"
        }

        Ui.Button {
            Layouts.Layout.alignment: QtQuick.Qt.AlignLeft
            Layouts.Layout.fillWidth: true

            flat: true
            font.weight: QtQuick.Font.Medium;
            icon.name: "document-open"
            icon.width: 32
            icon.height: 32
            text: "Open session"
        }

        Ui.Button {
            Layouts.Layout.alignment: QtQuick.Qt.AlignLeft
            Layouts.Layout.fillWidth: true

            flat: true
            font.weight: QtQuick.Font.Medium;
            icon.name: "document-import"
            icon.width: 32
            icon.height: 32
            text: "Import session"
        }

    }

    Ui.EditSeparator {
        Layouts.Layout.fillWidth: true
    }

    Ui.GroupBox {
        Layouts.Layout.alignment: QtQuick.Qt.AlignLeft
        title: "Open recent session"

        Ui.Button {
            Layouts.Layout.alignment: QtQuick.Qt.AlignLeft
            Layouts.Layout.fillWidth: true

            flat: true
            font.weight: QtQuick.Font.Medium;
            icon.name: "document-open-recent"
            icon.width: 32
            icon.height: 32
            palette.button: "transparent"
            text: "/home/devin/synthclone/stuff.session"
        }

    }

}
