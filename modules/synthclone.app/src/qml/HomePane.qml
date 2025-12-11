import QtQuick as QtQuick
import QtQuick.Layouts as Layouts

import Synthclone.Ui as Ui

Layouts.RowLayout {
    id: homePaneLayout
    spacing: 0

    Ui.VerticalTabBar {
        id: homeTabBar

        Layouts.Layout.alignment: QtQuick.Qt.AlignTop
        Layouts.Layout.preferredWidth: 175

        topPadding: 30

        Ui.VerticalTabButton {
            id: sessionsTabButton
            text: qsTr("Sessions")
        }

        Ui.VerticalTabButton {
            text: qsTr("Learn")
            enabled: false
        }

    }

    Ui.VerticalFrameBorder {
        id: homeFrameBorder
        Layouts.Layout.fillHeight: true
    }

    Ui.ContentPane {
        id: homeContentPane
        Layouts.Layout.fillHeight: true
        Layouts.Layout.fillWidth: true

        Layouts.StackLayout {
            anchors.fill: parent
            currentIndex: homeTabBar.currentIndex

            Ui.ScrollView {
                Layouts.Layout.fillHeight: true
                Layouts.Layout.fillWidth: true

                Ui.ScrollBar.horizontal.policy: Ui.ScrollBar.AlwaysOff
                Ui.ScrollBar.vertical.policy: Ui.ScrollBar.AsNeeded

                contentWidth: availableWidth

                implicitWidth: sessionsEditPane.implicitWidth
                implicitHeight: homeTabBar.implicitHeight

                Ui.EditPane {
                    id: sessionsEditPane
                    anchors.fill: parent

                    SessionsPane {}

                }

            }

            Ui.ScrollView {
                Layouts.Layout.fillHeight: true
                Layouts.Layout.fillWidth: true

                Ui.ScrollBar.horizontal.policy: Ui.ScrollBar.AlwaysOff
                Ui.ScrollBar.vertical.policy: Ui.ScrollBar.AsNeeded

                contentWidth: availableWidth

                implicitWidth: learnEditPane.implicitWidth
                implicitHeight: homeTabBar.implicitHeight

                Ui.EditPane {
                    id: learnEditPane
                    anchors.fill: parent

                    LearnPane {}

                }

            }

        }

    }

}
