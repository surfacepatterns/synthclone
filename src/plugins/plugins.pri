include(../../synthclone.pri)

################################################################################
# Build
################################################################################

isEmpty(BUILDDIR) {
    BUILDDIR = ../../../build
}

isEmpty(MAKEDIR) {
    MAKEDIR = ../../../make
}

CONFIG += plugin
DESTDIR = $${BUILDDIR}/lib/synthclone/plugins
INCLUDEPATH += ../../include
LIBS += -L$${BUILDDIR}/lib -lsynthclone
TEMPLATE = lib

################################################################################
# Install
################################################################################

target.path = $${SYNTHCLONE_PLUGIN_INSTALL_PATH}
INSTALLS += target
