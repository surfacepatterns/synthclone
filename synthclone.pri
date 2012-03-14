CONFIG += no_include_pwd

!isEmpty(DEBUG) {
    CONFIG += debug
}

isEmpty(MAJOR_VERSION) {
    MAJOR_VERSION = 0
}

isEmpty(MINOR_VERSION) {
    MINOR_VERSION = 1
}

isEmpty(REVISION) {
    REVISION = 10
}

macx {
    CONFIG += x86_64
    DEFINES += SYNTHCLONE_PLATFORM_MACX
    isEmpty(PREFIX) {
        PREFIX = /Applications
    }

    # For whatever reason, the build target will create these directories
    # auto-magically.
    #SYNTHCLONE_APP_SUFFIX = synthclone.app/Contents/MacOS
    SYNTHCLONE_APP_SUFFIX = .

    SYNTHCLONE_DATA_SUFFIX = synthclone.app/Contents/Resources
    SYNTHCLONE_DOC_SUFFIX = synthclone.app/Contents/Resources/devel-doc
    SYNTHCLONE_HEADER_SUFFIX = synthclone.app/Contents/Frameworks
    SYNTHCLONE_LIBRARY_SUFFIX = synthclone.app/Contents/PlugIns
    SYNTHCLONE_PLUGIN_SUFFIX = synthclone.app/Contents/PlugIns

    # Dependencies installed with MacPorts.  Is this the correct way to handle Mac?
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib
} else {

    unix:!macx {
        DEFINES += SYNTHCLONE_PLATFORM_UNIX
        isEmpty(PREFIX) {
            PREFIX = /usr/local
        }
    }

    win32 {
        DEFINES += SYNTHCLONE_PLATFORM_WIN32
        isEmpty(PREFIX) {
            PREFIX = C:/Program Files/synthclone
        }
    }

    SYNTHCLONE_APP_SUFFIX = bin
    SYNTHCLONE_DATA_SUFFIX = share/synthclone
    SYNTHCLONE_DOC_SUFFIX = share/doc/synthclone-devel
    SYNTHCLONE_HEADER_SUFFIX = include/synthclone
    SYNTHCLONE_LIBRARY_SUFFIX = lib
    SYNTHCLONE_PLUGIN_SUFFIX = lib/synthclone/plugins
}

SYNTHCLONE_APP_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_APP_SUFFIX}
SYNTHCLONE_DATA_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_DATA_SUFFIX}
SYNTHCLONE_DOC_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_DOC_SUFFIX}
SYNTHCLONE_HEADER_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_HEADER_SUFFIX}
SYNTHCLONE_LIBRARY_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_LIBRARY_SUFFIX}
SYNTHCLONE_PLUGIN_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_PLUGIN_SUFFIX}
