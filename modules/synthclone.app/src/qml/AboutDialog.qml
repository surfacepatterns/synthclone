import QtQuick as QtQuick
import QtQuick.Layouts as Layouts

import Synthclone.Ui as Ui

Ui.Dialog {
    id: root

    title: "About Synthclone"

    implicitWidth: 640

    buttons: [

        Ui.Button {
            text: qsTr("Close")
            icon.name: "window-close"
            Ui.DialogButtonBox.buttonRole: Ui.DialogButtonBox.AcceptRole

            onClicked: root.close()
        }

    ]

    Layouts.ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Ui.HorizontalTabBar {
            id: tabBar

            Layouts.Layout.alignment: QtQuick.Qt.AlignHCenter

            bottomPadding: 0
            topPadding: 10

            Ui.HorizontalTabButton {
                text: qsTr("Application")
            }

            Ui.HorizontalTabButton {
                text: qsTr("Build")
            }

            Ui.HorizontalTabButton {
                text: qsTr("License")
            }

        }

        Ui.HorizontalFrameBorder {
            Layouts.Layout.fillWidth: true
        }

        Ui.ContentPane {
            id: contentPane

            Layouts.Layout.fillHeight: true
            Layouts.Layout.fillWidth: true

            padding: 0

            Layouts.StackLayout {
                id: stackLayout

                anchors.fill: parent

                currentIndex: tabBar.currentIndex

                // XXX: We can likely do better than this.
                implicitHeight: 400

                Ui.ScrollView {
                    Layouts.Layout.fillWidth: true
                    Layouts.Layout.fillHeight: true

                    Ui.ScrollBar.horizontal.policy: Ui.ScrollBar.AlwaysOff
                    Ui.ScrollBar.vertical.policy: Ui.ScrollBar.AsNeeded

                    contentWidth: availableWidth

                    Ui.EditPane {
                        anchors.fill: parent

                        AboutApplicationPane {}

                    }

                }

                Ui.ScrollView {
                    Layouts.Layout.fillWidth: true
                    Layouts.Layout.fillHeight: true

                    Ui.ScrollBar.horizontal.policy: Ui.ScrollBar.AlwaysOff
                    Ui.ScrollBar.vertical.policy: Ui.ScrollBar.AsNeeded

                    contentWidth: availableWidth

                    Ui.EditPane {
                        anchors.fill: parent

                        AboutBuildPane {}

                    }

                }

                Ui.ScrollView {
                    Layouts.Layout.fillWidth: true
                    Layouts.Layout.fillHeight: true

                    Ui.ScrollBar.horizontal.policy: Ui.ScrollBar.AlwaysOff
                    Ui.ScrollBar.vertical.policy: Ui.ScrollBar.AsNeeded

                    contentWidth: availableWidth

                    Ui.EditPane {
                        anchors.fill: parent

                        AboutLicensePane {}

                    }

                }

            }

        }

    }

}
