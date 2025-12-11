import QtQuick as QtQuick
import QtQuick.Layouts as Layouts

import Synthclone.Ui as Ui

Layouts.GridLayout {

    columns: 2
    columnSpacing: 10
    rowSpacing: 5

    Ui.H1 {
        Layouts.Layout.columnSpan: 2
        text: "Build"
    }

    Ui.H2 {
        Layouts.Layout.columnSpan: 2
        text: "Environment"
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "System"
    }

    Ui.Label {
        objectName: "buildSystemLabel"
        wrapMode: Ui.Label.Wrap
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "Library Architecture"
    }

    Ui.Label {
        objectName: "buildLibraryArchitectureLabel"
        wrapMode: Ui.Label.Wrap
    }

    Ui.H2 {
        Layouts.Layout.columnSpan: 2
        text: "Tools"
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "CMake"
    }

    Ui.Label {
        objectName: "buildCMakeLabel"
        wrapMode: Ui.Label.Wrap
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "Generator"
    }

    Ui.Label {
        objectName: "buildGeneratorLabel"
        wrapMode: Ui.Label.Wrap
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "Compiler"
    }

    Ui.Label {
        objectName: "buildCompilerLabel"
        wrapMode: Ui.Label.Wrap
    }

    Ui.H2 {
        Layouts.Layout.columnSpan: 2
        text: "Configuration"
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "Build Type"
    }

    Ui.Label {
        objectName: "buildTypeLabel"
        wrapMode: Ui.Label.Wrap
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "C++ Flags"
    }

    Ui.Label {
        objectName: "buildCppFlagsLabel"
        wrapMode: Ui.Label.Wrap
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "C++ Standard"
    }

    Ui.Label {
        objectName: "buildCppStandardLabel"
        wrapMode: Ui.Label.Wrap
    }

    Ui.Label {
        font.weight: QtQuick.Font.Medium
        text: "Install Prefix"
    }

    Ui.Label {
        objectName: "buildInstallPrefixLabel"
        wrapMode: Ui.Label.Wrap
    }

}
