import QtQuick.Layouts as Layouts

import Synthclone.Ui as Ui

Layouts.ColumnLayout {
    id: root

    Ui.H1 {
        text: "Application"
    }

    Ui.Label {
        objectName: "applicationDescriptionLabel"

        Layouts.Layout.fillWidth: true
        Layouts.Layout.fillHeight: true

        // XXX: We can't use markdown text until this bug is fixed:
        //
        //     https://qt-project.atlassian.net/browse/QTBUG-142554
        //
        // textFormat: Ui.Label.MarkdownText

        wrapMode: Ui.Label.Wrap
    }

}
