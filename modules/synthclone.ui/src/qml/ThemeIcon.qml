import QtQuick as QtQuick

QtQuick.Item {
    id: root

    // XXX: Use the C++ enums.

    enum Mode {
        Normal = 0,
        Disabled = 1,
        Active = 2,
        Selected = 3
    }

    enum State {
        On = 0,
        Off = 1
    }

    // XXX: We don't have properties that mirror the functionality of
    // `icon.color` or `icon.source`; as of now, we don't make use of either
    // property.  If and when we use non-theme icons, we can implement
    // `icon.source` and rename this class to `Icon`.
    //
    // `icon.color` is slightly more difficult:
    //
    // https://github.com/qt/qtdeclarative/blob/\
    // ce63ad2d4e5596e6151ea08c2d42adb6a1aeb85a/src/quickcontrolsimpl/\
    // qquickiconimage.cpp#L191-L194

    property alias asynchronous: image.asynchronous
    property alias cache: image.cache
    property alias fillMode: image.fillMode
    property alias horizontalAlignment: image.horizontalAlignment
    property alias mipmap: image.mipmap
    property alias mirrorHorizontally: image.mirror
    property alias mirrorVertically: image.mirrorVertically
    property alias paintedHeight: image.paintedHeight
    property alias paintedWidth: image.paintedWidth
    property alias retainWhileLoading: image.retainWhileLoading
    property alias smooth: image.smooth
    property alias sourceClipRect: image.sourceClipRect
    property alias sourceSize: image.sourceSize
    property alias status: image.status
    property alias verticalAlignment: image.verticalAlignment

    // XXX: There seems to be a QML bug that causes a crash when using
    // `Qt.enumValueToString()`.  I haven't really looked into what the culprit
    // might be.  I'm maintaining these explicit string lookup tables for now.
    readonly property list<string> _modeStrings: [
        "Normal",
        "Disabled",
        "Active",
        "Selected"
    ]

    readonly property list<string> _stateStrings: [
        "On",
        "Off"
    ]

    property int iconState: ThemeIcon.State.Off
    property int mode: ThemeIcon.Mode.Normal
    property string name: ""

    property string _encodedName: encodeURIComponent(root.name)
    property string _modeString: root._modeStrings[root.mode]
    property string _stateString: root._stateStrings[root.iconState]

    implicitHeight: image.implicitHeight
    implicitWidth: image.implicitWidth

    QtQuick.Image {
        id: image

        anchors.fill: root

        source:
            root.name ?
            `image://themeicon/${root._encodedName}?mode=${root._modeString}\
&state=${root._stateString}` :
            ""

    }

}
