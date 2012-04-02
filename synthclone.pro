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
    documentationBuilder.CONFIG += target_predeps
    documentationBuilder.clean = resources/devel-docs
    documentationBuilder.commands = ./install/build-api-docs
    documentationBuilder.name = build-api-docs
    documentationBuilder.output = resources/devel-docs
    QMAKE_EXTRA_COMPILERS += documentationBuilder

    htmlDocumentation.CONFIG += no_check_exist
    htmlDocumentation.files = resources/devel-doc/html/*
    htmlDocumentation.path = $${SYNTHCLONE_LIBRARY_DOC_INSTALL_PATH}/html
    INSTALLS += htmlDocumentation

    htmlSearchDocumentation.CONFIG += no_check_exist
    htmlSearchDocumentation.files = resources/devel-doc/html/search/*
    htmlSearchDocumentation.path = $${SYNTHCLONE_LIBRARY_DOC_INSTALL_PATH}/html/search
    INSTALLS += htmlSearchDocumentation

    latexDocumentation.CONFIG += no_check_exist
    latexDocumentation.files = resources/devel-doc/latex/*
    latexDocumentation.path = $${SYNTHCLONE_LIBRARY_DOC_INSTALL_PATH}/latex
    INSTALLS += latexDocumentation
}

icon.files = src/lib/images/32x32/synthclone.png

macx {
    icon.path = $${SYNTHCLONE_DATA_INSTALL_PATH}
}

unix:!macx {
    icon.path = $${PREFIX}/share/icons/

    desktop.CONFIG += no_check_exist
    desktop.extra = ./install/build-desktop-file --prefix='$${PREFIX}'
    desktop.files = resources/synthclone.desktop
    desktop.path = $${PREFIX}/share/applications
    INSTALLS += desktop

    pkgconfig.CONFIG += no_check_exist
    pkgconfig.extra = ./install/build-pkgconfig --prefix='$${PREFIX}'
    pkgconfig.files = resources/synthclone.pc
    pkgconfig.path = $${PREFIX}/lib/pkgconfig
    INSTALLS += pkgconfig
}

INSTALLS += icon
