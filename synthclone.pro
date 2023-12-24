include(synthclone.pri)

###############################################################################
# Build
###############################################################################

isEmpty(BUILDDIR) {
    BUILDDIR = build
}

SUBDIRS = src
TEMPLATE = subdirs

###############################################################################
# Install
###############################################################################

isEmpty(SKIP_API_DOCS) {
    documentation.CONFIG += directory no_check_exist
    documentation.extra = ./install/build-api-docs
    documentation.files = resources/documentation/api/html \
        resources/documentation/api/latex
    documentation.path = \
        $${SYNTHCLONE_DOC_INSTALL_PATH}/synthclone-$${SYNTHCLONE_VERSION}/api
    INSTALLS += documentation
}

icon.files = src/lib/images/32x32/synthclone.png

unix:!macx {
    icon.path = $${SYNTHCLONE_DATA_INSTALL_PATH}/icons/

    desktop.CONFIG += no_check_exist
    desktop.extra = ./install/build-desktop-file \
        '$${SYNTHCLONE_APP_INSTALL_PATH}' '$${SYNTHCLONE_DATA_INSTALL_PATH}'
    desktop.files = resources/synthclone.desktop
    desktop.path = $${SYNTHCLONE_DATA_INSTALL_PATH}/applications
    INSTALLS += desktop

    pkgconfig.CONFIG += no_check_exist
    pkgconfig.extra = ./install/build-pkgconfig '$${INCLUDEDIR}' \
        '$${SYNTHCLONE_LIBRARY_INSTALL_PATH}'
    pkgconfig.files = resources/synthclone.pc
    pkgconfig.path = $${SYNTHCLONE_LIBRARY_INSTALL_PATH}/pkgconfig
    INSTALLS += pkgconfig
} else {
    icon.path = $${SYNTHCLONE_DATA_INSTALL_PATH}
}
INSTALLS += icon
