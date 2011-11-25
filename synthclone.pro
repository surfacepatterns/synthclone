include(synthclone.pri)

###############################################################################
# Build
###############################################################################

SUBDIRS = src
TEMPLATE = subdirs

###############################################################################
# Install
###############################################################################

isEmpty(SKIP_API_DOCS) {
    documentation.files = $${BUILDDIR}/share/doc/synthclone-devel/*
    documentation.path = $${PREFIX}/share/doc/synthclone-devel
    INSTALLS += documentation
}
