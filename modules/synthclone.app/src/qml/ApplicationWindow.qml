import QtQuick as QtQuick
import QtQuick.Layouts as Layouts

import Synthclone.Ui as Ui

Ui.ApplicationWindow {
    id: root

    width: 1024
    height: 768
    minimumHeight:
        menuBar.implicitHeight + mainTabBar.implicitHeight +
        mainLayout.implicitHeight
    minimumWidth:
        Math.max(
            menuBar.implicitWidth, mainTabBar.implicitWidth,
            mainLayout.implicitWidth)
    visible: false
    title: qsTr("synthclone")

    menuBar: Ui.MenuBar {

        Ui.Menu {
            id: fileMenu

            title: qsTr("File")

            Ui.MenuItem {
                id: newSessionMenuItem

                objectName: "newSessionMenuItem"
                shortcuts: [QtQuick.StandardKey.New]
                text: qsTr("New Session")
                icon.name: "document-new"
            }

            Ui.MenuItem {
                objectName: "openSessionMenuItem"
                shortcuts: [QtQuick.StandardKey.Open]
                text: qsTr("Open Session")
                icon.name: "document-open"
            }

            Ui.MenuItem {
                objectName: "openRecentSessionMenuItem"
                text: qsTr("Open Recent Session")
                icon.name: "document-open-recent"
            }

            Ui.MenuSeparator {}

            Ui.MenuItem {
                objectName: "importSessionMenuItem"
                text: qsTr("Import Session")
                icon.name: "document-import"
            }

            Ui.MenuSeparator {}

            Ui.MenuItem {
                objectName: "saveSessionMenuItem"
                shortcuts: [QtQuick.StandardKey.Save]
                text: qsTr("Save Session")
                icon.name: "document-save"
            }

            Ui.MenuItem {
                objectName: "saveSessionAsMenuItem"
                shortcuts: [QtQuick.StandardKey.SaveAs]
                text: qsTr("Save Session As ...")
                icon.name: "document-save-as"
            }

            Ui.MenuSeparator {}

            Ui.MenuItem {
                objectName: "exportMenuItem"
                text: qsTr("Export Session")
                icon.name: "document-export"
            }

            Ui.MenuSeparator {}

            Ui.MenuItem {
                objectName: "closeSessionMenuItem"
                shortcuts: [QtQuick.StandardKey.Close]
                text: qsTr("Close Session")
                icon.name: "document-cleanup"
            }

            Ui.MenuItem {
                objectName: "quitMenuItem"
                shortcuts: [QtQuick.StandardKey.Quit]
                text: qsTr("Quit")
                icon.name: "application-exit"
            }

        }

        Ui.Menu {
            title: qsTr("Edit")
        }

        Ui.Menu {
            title: qsTr("View")
        }

        Ui.Menu {
            title: qsTr("Help")

            Ui.MenuItem {
                objectName: "aboutSynthcloneMenuItem"
                text: qsTr("About synthclone")
                icon.name: "help-about"
            }

            Ui.MenuItem {
                objectName: "aboutQtMenuItem"
                text: qsTr("About Qt")
            }

        }

    }

    header: Ui.HorizontalTabBar {
        id: mainTabBar

        topPadding: 15

        Ui.HorizontalTabButton {
            text: qsTr("Home")
        }

        Ui.HorizontalTabButton {
            text: qsTr("Session")
            enabled: false
        }

        Ui.HorizontalTabButton {
            text: qsTr("Export")
            enabled: false
        }

    }

    Layouts.ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: 0

        Ui.HorizontalFrameBorder {
            Layouts.Layout.fillWidth: true
        }

        Layouts.StackLayout {
            Layouts.Layout.fillHeight: true
            Layouts.Layout.fillWidth: true

            currentIndex: mainTabBar.currentIndex

            HomePane {
                Layouts.Layout.fillHeight: true
                Layouts.Layout.fillWidth: true
            }

            SessionPane {
                Layouts.Layout.fillHeight: true
                Layouts.Layout.fillWidth: true
            }

            ExportPane {
                Layouts.Layout.fillHeight: true
                Layouts.Layout.fillWidth: true
            }

        }

    }

    AboutDialog {
        objectName: "aboutDialog"
    }

}
