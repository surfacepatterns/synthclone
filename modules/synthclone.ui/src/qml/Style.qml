pragma ComponentBehavior: Bound
pragma Singleton

import QtQuick as QtQuick

QtQuick.QtObject {
    id: style

    component Palette: QtQuick.Palette {
        id: palette

        // The `Palette` class ignores direct assignment to color groups.  This
        // is almost certainly a QML bug.
        //
        // For now, we just manipulate the color groups that are already there.

        readonly property QtQuick.SystemPalette _activeSystemPalette:
            QtQuick.SystemPalette {
                colorGroup: QtQuick.SystemPalette.Active
            }
        readonly property QtQuick.SystemPalette _disabledSystemPalette:
            QtQuick.SystemPalette {
                colorGroup: QtQuick.SystemPalette.Disabled
            }
        readonly property QtQuick.SystemPalette _inactiveSystemPalette:
            QtQuick.SystemPalette {
                colorGroup: QtQuick.SystemPalette.Inactive
            }

        function _getButtonColor(p: QtQuick.SystemPalette): QtQuick.color {
            return style.darkMode ? QtQuick.Qt.lighter(p.button, 1.15) :
                QtQuick.Qt.darker(p.button, 1.08);
        }

        active.button: palette._getButtonColor(palette._activeSystemPalette)
        disabled.button:
            palette._getButtonColor(palette._disabledSystemPalette)
        inactive.button:
            palette._getButtonColor(palette._inactiveSystemPalette)

    }

    property int _contrastPreference:
        QtQuick.Application.styleHints.accessibility.contrastPreference
    property int _colorScheme:
        QtQuick.Application.styleHints.colorScheme

    readonly property Palette palette: Palette {
        id: palette
    }

    property bool darkMode: {
        switch (style._colorScheme) {
        case QtQuick.Qt.Dark:
            return true;
        case QtQuick.Qt.Light:
            return false;
        default:
            var p = style.palette
            return p.windowText.hslLightness < p.window.hslLightness;
        }
    }

    function getContentColor(palette: QtQuick.Palette): QtQuick.color {
        return style.darkMode ? QtQuick.Qt.lighter(palette.window, 1.4) :
            QtQuick.Qt.darker(palette.window, 1.02);
    }

    function getOutlineColor(palette: QtQuick.Palette): QtQuick.color {
        return QtQuick.Qt.darker(palette.dark, 1.1);
    }

}
