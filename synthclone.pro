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
    documentation.files = $${BUILDDIR}/$${SYNTHCLONE_LIBRARY_DOC_SUFFIX}/*
    documentation.path = $${SYNTHCLONE_LIBRARY_DOC_INSTALL_PATH}
    INSTALLS += documentation
}
