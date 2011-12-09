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
    REVISION = 9
}

macx {
    DEFINES += SYNTHCLONE_PLATFORM_MACX
    isEmpty(PREFIX) {
        PREFIX = /Applications
    }
    SYNTHCLONE_APP_INSTALL_PATH = $${PREFIX}/synthclone.app/Contents/MacOS
    SYNTHCLONE_DATA_INSTALL_PATH = $${PREFIX}/synthclone.app/Contents/Resources
    SYNTHCLONE_DOC_INSTALL_PATH = $${PREFIX}/synthclone.app/Contents/Resources/devel-doc
    SYNTHCLONE_HEADER_INSTALL_PATH = $${PREFIX}/synthclone.app/Contents/Frameworks
    SYNTHCLONE_LIBRARY_INSTALL_PATH = $${PREFIX}/synthclone.app/Contents/PlugIns
    SYNTHCLONE_PLUGIN_INSTALL_PATH = $${PREFIX}/synthclone.app/Contents/PlugIns
} else {
    unix {
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
    SYNTHCLONE_APP_INSTALL_PATH = $${PREFIX}/bin
    SYNTHCLONE_DATA_INSTALL_PATH = $${PREFIX}/share/synthclone
    SYNTHCLONE_DOC_INSTALL_PATH = $${PREFIX}/share/doc/synthclone-devel
    SYNTHCLONE_HEADER_INSTALL_PATH = $${PREFIX}/include
    SYNTHCLONE_LIBRARY_INSTALL_PATH = $${PREFIX}/lib
    SYNTHCLONE_PLUGIN_INSTALL_PATH = $${PREFIX}/lib/synthclone/plugins
}
