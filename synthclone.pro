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
    documentation.extra = ./install/build-api-docs
    documentation.files = resources/devel-doc/*
    documentation.path = $${SYNTHCLONE_LIBRARY_DOC_INSTALL_PATH}
    INSTALLS += documentation
}

icon.files = src/lib/images/32x32/synthclone.png

macx {
    icon.path = $${SYNTHCLONE_DATA_INSTALL_PATH}
}

unix:!macx {
    icon.path = $${PREFIX}/share/icons/

    desktop.extra = "./install/build-desktop-file --prefix='$${PREFIX}'"
    desktop.files = resources/synthclone.desktop
    desktop.path = $${PREFIX}/share/applications
    INSTALLS += desktop

    pkgconfig.extra = "./install/build-pkgconfig --prefix='$${PREFIX}'"
    pkgconfig.files = resources/synthclone.pc
    pkgconfig.path = $${PREFIX}/lib/pkgconfig
    INSTALLS += pkgconfig
}

INSTALLS += icon
