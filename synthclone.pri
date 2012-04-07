CONFIG += no_include_pwd warn_on

!isEmpty(DEBUG) {
    CONFIG += debug
}

isEmpty(MAJOR_VERSION) {
    MAJOR_VERSION = 0
}

isEmpty(MINOR_VERSION) {
    MINOR_VERSION = 2
}

isEmpty(REVISION) {
    REVISION = 0
}

SYNTHCLONE_VERSION = $${MAJOR_VERSION}.$${MINOR_VERSION}.$${REVISION}

macx {
    CONFIG += x86_64
    DEFINES += SYNTHCLONE_PLATFORM_MACX
    isEmpty(PREFIX) {
        PREFIX = ""
    }

    # The application is handled using a Mac application bundle.
    SYNTHCLONE_APP_SUFFIX = Applications

    # Headers and libraries are handled using a Mac framework.
    SYNTHCLONE_HEADER_SUFFIX = Library/Frameworks
    SYNTHCLONE_LIBRARY_SUFFIX = Library/Frameworks

    SYNTHCLONE_LIBRARY_DOC_SUFFIX = Library/Frameworks/synthclone.framework/Documentation

    SYNTHCLONE_DATA_SUFFIX = Applications/synthclone.app/Contents/Resources
    SYNTHCLONE_PLUGIN_SUFFIX = Applications/synthclone.app/Contents/PlugIns
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
    SYNTHCLONE_HEADER_SUFFIX = include/synthclone
    SYNTHCLONE_LIBRARY_SUFFIX = lib
    SYNTHCLONE_LIBRARY_DOC_SUFFIX = share/doc/synthclone-devel
    SYNTHCLONE_PLUGIN_SUFFIX = lib/synthclone/plugins
}

SYNTHCLONE_APP_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_APP_SUFFIX}
SYNTHCLONE_DATA_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_DATA_SUFFIX}
SYNTHCLONE_HEADER_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_HEADER_SUFFIX}
SYNTHCLONE_LIBRARY_DOC_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_LIBRARY_DOC_SUFFIX}
SYNTHCLONE_LIBRARY_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_LIBRARY_SUFFIX}
SYNTHCLONE_PLUGIN_INSTALL_PATH = $${PREFIX}/$${SYNTHCLONE_PLUGIN_SUFFIX}
